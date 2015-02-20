#include "Module.h"
#include "Wire.h"
#include "Modulo.h"


uint8_t Module::_lastAssignedAddress = 9;

Module::Module(const char *deviceType) :
    _deviceType(deviceType), _deviceID(0xFFFF), _address(0xFF) {
}

Module::Module(const char *deviceType, uint16_t deviceID) : 
    _deviceType(deviceType), _deviceID(deviceID), _address(0xFF) {
}

uint16_t Module::getDeviceID() {
    return _deviceID;
}

uint8_t Module::getAddress() {
    _initAddress();
    return _address;
}

void Module::_initAddress() {
    if (_address != 0xFF) {
        return;
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

        ModuloSetup();
        ModuloGlobalReset();
        _moduloInitialized = true;
    }
    
    if (_deviceID == 0xFFFF) {
        // Find the first device with the specified type and no assigned address
        uint16_t deviceID = ModuloGetNextDeviceID(0);
        while (deviceID != 0xFFFF) {
            char deviceType[32] = {0};
            ModuloGetDeviceType(deviceID, deviceType, 31);
            if (strcmp(deviceType,_deviceType) == 0) {
                _deviceID = deviceID;
                break;
            }
            
            deviceID = ModuloGetNextDeviceID(deviceID+1);
        }
    }

    if (_deviceID == 0xFFFF) {
        // Couldn't find a device
        return;
    }
    
    _address = ModuloGetAddress(_deviceID);
    if (_address == 0) {
        _address = ++_lastAssignedAddress;
        ModuloSetAddress(_deviceID, _address);
    }
}
