#include "Broadcast.h"
#include "Modulo.h"
#include "Arduino.h"

#define BroadcastAddress 9

#define BroadcastCommandGlobalReset 0
#define BroadcastCommandGetNextDeviceID 1
#define BroadcastCommandSetAddress 2
#define BroadcastCommandGetAddress 3
#define BroadcastCommandGetDeviceType 4
#define BroadcastCommandGetDeviceVersion 5
#define BroadcastCommandGetCompanyName 6
#define BroadcastCommandGetProductName 7
#define BroadcastCommandGetDocURL 8
#define BroadcastCommandGetDocURLContinued 9
#define BroadcastCommandGetInterrupt 10
#define BroadcastCommandSetStatusLED 11

void ModuloGlobalReset() {
    moduloTransfer(BroadcastAddress, BroadcastCommandGlobalReset,
                   0, 0, 0, 0, 1);
}

uint16_t ModuloGetNextDeviceID(uint16_t lastDeviceID) {
    uint8_t sendData[2] = {lastDeviceID & 0xFF, lastDeviceID >> 8 };
    uint8_t receiveData[2] = {0xFF,0xFF};
    if (!moduloTransfer(BroadcastAddress, BroadcastCommandGetNextDeviceID,
                        sendData, 2, receiveData, 2, 1)) {
        return 0xFFFF;
    }
    return receiveData[1] | (receiveData[0] << 8);
}

bool ModuloSetAddress(uint16_t deviceID, uint8_t address) {
    uint8_t sendData[3] = {deviceID & 0xFF, deviceID >> 8, address};
    return moduloTransfer(BroadcastAddress, BroadcastCommandSetAddress,
                          sendData, 3, 0, 0, 1);
}

uint8_t ModuloGetAddress(uint16_t deviceID) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    uint8_t address = 0;
    if (moduloTransfer(BroadcastAddress, BroadcastCommandGetAddress,
                       sendData, 2, &address, 1, 1)){
        return address;
    }
    return 0;
}

bool ModuloGetDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetDeviceType,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloGetCompanyName(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetCompanyName,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloGetProductName(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetProductName,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloGetDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetDocURL,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}
