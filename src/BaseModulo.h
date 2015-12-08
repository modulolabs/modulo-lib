#ifndef MODULO_BASE_H
#define MODULO_BASE_H

#include <inttypes.h>
class _Modulo;

/// The base class for all modulos. 
/// Handles common functionality like discovering devices and assigning an address.
class BaseModulo {
 public:
    virtual ~BaseModulo();

    /// Return the i2c address, or 0xFF if no device could be found
    uint8_t getAddress();

    /// Return the device ID, or 0xFFFF if no device could be found
    uint16_t getDeviceID();
    
    /// Set the ID of the modulo that this object should connect to.
    void setDeviceID(uint16_t deviceID);

    // If a BaseModulo exists that has the specified deviceID, find and return
    // it.
    static BaseModulo* findByDeviceID(uint16_t deviceID);

    // Called by _Modulo.loop() on every iteration.
    static void loop();

 protected:
    BaseModulo(const char *deviceType);
    BaseModulo(const char *deviceType, uint16_t deviceID);
    
    // Initialize the module. Return true if initialization took place, false if
    // it failed or if the module was already initialized.
    virtual bool _init();

    friend _Modulo;

    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);
    
    static void _globalReset();

    bool _transfer(uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, uint8_t retries=3);


 private:
    static uint8_t _lastAssignedAddress;

    const char *_deviceType;
    uint16_t _deviceID;
    uint8_t _address;
    bool _disconnected;

    // Linked list of modules
    static BaseModulo *_firstBaseModulo;
    BaseModulo *_nextBaseModulo;
};

#endif
