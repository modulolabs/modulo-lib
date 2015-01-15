#ifndef MODULO_BASE_H
#define MODULO_BASE_H

#include <inttypes.h>

/// The base class for all modules
class Module {
 public:
    /// Return the i2c address, or 0xFF if no device could be found
    uint8_t getAddress();

    /// Return the device ID, or 0xFFFF if no device could be found
    uint16_t getDeviceID();
    
 protected:
    Module(const char *deviceType);
    Module(const char *deviceType, uint16_t deviceID);

    void _initAddress();
    
 private:
    static uint8_t _lastAssignedAddress;

    const char *_deviceType;
    uint16_t _deviceID;
    uint8_t _address;
};

    
#endif
