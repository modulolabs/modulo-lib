#include "Module.h"
#include "Modulo.h"

#ifdef SPARK
#include <spark_wiring.h>
#else
#include "Arduino.h"
#endif

uint8_t ModuloBase::_lastAssignedAddress = 9;
ModuloBase* ModuloBase::_firstModuloBase = NULL;

ModuloBase::ModuloBase(const char *deviceType) :
    _deviceType(deviceType), _deviceID(0xFFFF), _address(0xFF), _disconnected(true) {

    _nextModuloBase = _firstModuloBase;
    _firstModuloBase = this;
}

ModuloBase::ModuloBase(const char *deviceType, uint16_t deviceID) :
    _deviceType(deviceType), _deviceID(deviceID), _address(0xFF), _disconnected(true) {

    _nextModuloBase = _firstModuloBase;
    _firstModuloBase = this;
}

ModuloBase::~ModuloBase() {

    // Remove this module from the linked list
    ModuloBase *prev = NULL;
    for (ModuloBase *m = _firstModuloBase; m ; m = m->_nextModuloBase) {
        if (m == this) {
            if (prev) {
                prev->_nextModuloBase = _nextModuloBase;
            } else {
                _firstModuloBase = _nextModuloBase;
            }
        }
    }
}

ModuloBase *ModuloBase::findByDeviceID(uint16_t deviceID) {
    for (ModuloBase *m = _firstModuloBase; m ; m = m->_nextModuloBase) {
        if (m->getDeviceID() == deviceID) {
            return m;
        }
    }
    return NULL;
}

void ModuloBase::_reset() {
    _address = 0xFF;
}

void ModuloBase::_globalReset() {
    for (ModuloBase *m = _firstModuloBase; m ; m = m->_nextModuloBase) {
        m->_reset();
    }
}

void ModuloBase::_processEvent(uint8_t eventCode, uint16_t eventData) {
}

uint16_t ModuloBase::getDeviceID() {
    _init();
    return _deviceID;
}

uint8_t ModuloBase::getAddress() {
    _init();
    return _address;
}

void ModuloBase::loop() {
    for (ModuloBase *m = _firstModuloBase; m ; m = m->_nextModuloBase) {
        m->_loop();
    }
}

void ModuloBase::_loop() {
    if (_disconnected) {
        if (getAddress() != 0xFF) {
            _disconnected = false;
        }
    }
}

bool ModuloBase::_init() {
    if (_address != 0xFF) {
        return false;
    }

    // Ensure that a global reset has been performed
    static bool _moduloInitialized = false;
    if (!_moduloInitialized) {

        // Wait until at least 100ms after startup
        // so that connected devices can initialize
        unsigned long t = millis();
        if (t < 100) {
            delay(100-t);
        }

        Modulo.setup();
        Modulo.globalReset();
        _moduloInitialized = true;
    }

    if (_deviceID == 0xFFFF) {
        // Find the first device with the specified type and no assigned address
        uint16_t deviceID = Modulo.getNextDeviceID(0);
        while (deviceID != 0xFFFF) {

            // First look for a ModuloBase that already has this deviceID
            ModuloBase *m = _firstModuloBase;
            for (; m && m->_deviceID != deviceID; m = m->_nextModuloBase) {
            }

            if (m == NULL) {
                char deviceType[32] = {0};
                Modulo.getDeviceType(deviceID, deviceType, 31);
                if (strcmp(deviceType,_deviceType) == 0) {
                    _deviceID = deviceID;
                    break;
                }
            }

            deviceID = Modulo.getNextDeviceID(deviceID+1);
        }
    }

    if (_deviceID == 0xFFFF) {
        // Couldn't find a device
        return false;
    }

    _address = Modulo.getAddress(_deviceID);
    if (_address == 0) {
        _address = ++_lastAssignedAddress;
        Modulo.setAddress(_deviceID, _address);
    }
    return true;
}

bool ModuloBase::_transfer(uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{
    if (_disconnected) {
        return false;
    }

    for (int i=0; i < retries; i++) {
        // Unable to assign an address. Return false.
        uint8_t address = getAddress();
        if (address == 0xFF) {
            continue;
        }

        // We have a valid address, attempt the transfer.
        if (Modulo.transfer(address, command, sendData, sendLen,
            receiveData, receiveLen)) {
            return true;
        }

        // If the transfer failed, try to re-assign an address. The device may
        // have been removed and re-connected
        Modulo.setAddress(_deviceID, _address);
    }


    _disconnected = true;
    return false;
}
