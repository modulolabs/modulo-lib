#ifndef MODULO_BASE_H
#define MODULO_BASE_H

#include <inttypes.h>

void ModuloSetHighBitRate();

class ModBase {
 public:
    ModBase(const char *deviceType);
    ModBase(const char *deviceType, uint16_t deviceID);

    uint8_t getAddress();
    uint16_t getDeviceID();
    
 protected:
    void _initAddress();
    
 private:
    static uint8_t _lastAssignedAddress;

    const char *_deviceType;
    uint16_t _deviceID;
    uint8_t _address;
};


bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries=3);
    
#endif
