#ifndef MODULO_BASE_H
#define MODULO_BASE_H

#include <inttypes.h>
class _Modulo;

/// The base class for all modules
class ModuloBase {
 public:
    /// Return the i2c address, or 0xFF if no device could be found
    uint8_t getAddress();

    /// Return the device ID, or 0xFFFF if no device could be found
    uint16_t getDeviceID();
    

    static ModuloBase* findByDeviceID(uint16_t deviceID);

    static void loop();

 protected:
    ModuloBase(const char *deviceType);
    ModuloBase(const char *deviceType, uint16_t deviceID);
    
    virtual ~ModuloBase();

    // Initialize the module. Return true if initialization took place, false if
    // it failed or if the module was already initialized.
    virtual bool _init();
    virtual void _reset();
    virtual void _loop();

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
    static ModuloBase *_firstModuloBase;
    ModuloBase *_nextModuloBase;
};

#endif
