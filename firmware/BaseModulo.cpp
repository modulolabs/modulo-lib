#include "BaseModulo.h"
#include "Modulo.h"

#ifdef SPARK
#include <spark_wiring.h>
#else
#include "Arduino.h"
#endif

uint8_t BaseModulo::_lastAssignedAddress = 9;
BaseModulo* BaseModulo::_firstBaseModulo = NULL;

BaseModulo::BaseModulo(const char *deviceType) :
    _deviceType(deviceType), _deviceID(0xFFFF), _address(0xFF), _disconnected(false) {

    _nextBaseModulo = _firstBaseModulo;
    _firstBaseModulo = this;
}

BaseModulo::BaseModulo(const char *deviceType, uint16_t deviceID) :
    _deviceType(deviceType), _deviceID(deviceID), _address(0xFF), _disconnected(false) {

    _nextBaseModulo = _firstBaseModulo;
    _firstBaseModulo = this;
}

BaseModulo::~BaseModulo() {
    // Remove this module from the linked list
    BaseModulo *prev = NULL;
    for (BaseModulo *m = _firstBaseModulo; m ; m = m->_nextBaseModulo) {
        if (m == this) {
            if (prev) {
                prev->_nextBaseModulo = _nextBaseModulo;
            } else {
                _firstBaseModulo = _nextBaseModulo;
            }
        }
    }
}

BaseModulo *BaseModulo::findByDeviceID(uint16_t deviceID) {
    for (BaseModulo *m = _firstBaseModulo; m ; m = m->_nextBaseModulo) {
        if (m->getDeviceID() == deviceID) {
            return m;
        }
    }
    return NULL;
}

void BaseModulo::_globalReset() {
    for (BaseModulo *m = _firstBaseModulo; m ; m = m->_nextBaseModulo) {
        m->_address = 0xFF;
    }
}

void BaseModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {
}

uint16_t BaseModulo::getDeviceID() {
    return _deviceID;
}

void BaseModulo::setDeviceID(uint16_t deviceID) {
    if (deviceID != _deviceID) {
        _disconnected = true;
        _deviceID = deviceID;
        _address = 0xFF;
    }
}


uint8_t BaseModulo::getAddress() {
    return _address;
}

void BaseModulo::loop() {

    // If a device has a valid ID but is disconnected, reset its address
    // so it will be picked up again. This is necessary when devices get
    // disconnected but don't actually lose power.
    for (BaseModulo *m = _firstBaseModulo; m ; m = m->_nextBaseModulo) {
        if (m->_disconnected and m->_deviceID != 0xFFFF) {
            Modulo.setAddress(m->_deviceID, 0);
        }
    }

    // Handle any newly connected devices
    uint16_t deviceID = Modulo.getNextUnassignedDeviceID(0);
    while (deviceID != 0xFFFF) {

        // First look for a BaseModulo that already has this deviceID
        BaseModulo *m = _firstBaseModulo;
        for (; m && m->_deviceID != deviceID; m = m->_nextBaseModulo) {
        }

        // If we couldn't find a base with the device ID, look for one with no
        // device ID but the correct device type
        if (m == NULL) {
            char deviceType[32] = {0};
            Modulo.getDeviceType(deviceID, deviceType, 31);

            for (m = _firstBaseModulo; m ; m = m->_nextBaseModulo) {
                if (m->_deviceID == 0xFFFF and (strcmp(deviceType,m->_deviceType) == 0)) {
                    break;
                }
            }
        }
    

        if (m) {
            // Found a BaseModulo. Assign it.
            m->_deviceID = deviceID;
            m->_disconnected = false;

            // Allocate a new address if necessary
            if (m->_address == 0xFF) {
                m->_address = ++_lastAssignedAddress;;
            }
            Modulo.setAddress(deviceID, m->_address );

            m->_init();
        } else {
            // Give it an unused but valid address
            Modulo.setAddress(deviceID, 127);
        }

        deviceID = Modulo.getNextUnassignedDeviceID(deviceID);
    }
}

bool BaseModulo::_init() {
    return true;
}

bool BaseModulo::_transfer(uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{
    if (_disconnected or _address == 0xFF) {
        return false;
    }

    // Try the transfer several times in case noise causes a failure.
    for (int i=0; i < retries; i++) {
        if (Modulo.transfer(_address, command, sendData, sendLen,
            receiveData, receiveLen)) {
            return true;
        }
    }

    // If the transfer failed after several retries, then it's disconnected.
    _disconnected = true;
    return false;
}
