#include <Wire.h>
#include "Modulo.h"
#include "Arduino.h"
#include "DallasTemperature.h"

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

#define ControllerFunctionReadTemperatureProbe 0

void ModuloSetHighBitRate() {
    TWBR = 6;
    TWSR &= ~(3);
}

uint8_t
_crc8_ccitt_update (uint8_t inCrc, uint8_t inData)
{
    uint8_t i;
    uint8_t data;
    data = inCrc ^ inData;
    for ( i = 0; i < 8; i++ ) {
        if (( data & 0x80 ) != 0 ) {
            data <<= 1;
            data ^= 0x07;
        } else {
            data <<= 1;
        }
    }
    return data;
}


bool _moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen)
{
    // Star the transmit CRC with the address in the upper 7 bits
    uint8_t crc =  _crc8_ccitt_update(0, address << 1);
    
    Wire.beginTransmission(address);

    // Send the command and length
    Wire.write(command);
    Wire.write(sendLen);
    crc = _crc8_ccitt_update(crc, command);
    crc = _crc8_ccitt_update(crc, sendLen);
    
    // Send the data
    for (int i=0; i < sendLen; i++) {
        Wire.write(sendData[i]);
        crc = _crc8_ccitt_update(crc, sendData[i]);
    }

    // Send the CRC and end the transmission
    Wire.write(crc);
    if (Wire.endTransmission() != 0) {
        return false;
    }

    if (receiveLen == 0) {
        return true;
    }
    
    // Request receiveLen data bytes plus 1 CRC byte.
    if (Wire.requestFrom((int)address, (int)receiveLen+1) != receiveLen+1) {
        return false;
    }
    
    // Start the CRC with the I2C address byte (address in upper 7, 1 in lsb)
    crc = _crc8_ccitt_update(0, address << 1 | 1);
    
    // Receive the data
    for (int i=0; i < 32 and i < receiveLen; i++) {
        receiveData[i] = Wire.read();
        
        // XXX: Hack to detect the end of variable length strings.
        if (i > 0 and receiveLen == 31 and
            receiveData[i-1] == 0 and receiveData[i] == crc) {
            return true;
        }
        crc = _crc8_ccitt_update(crc, receiveData[i]);
    }
    
    // Receive the CRC.
    uint8_t receivedCRC = Wire.read();

    // Check the CRC.
    return (crc == receivedCRC);
}

static uint8_t _controllerAddress = 0;

static bool _handleControllerBroadcastTransfer(
    uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen)
{
    switch (command) {
    case BroadcastCommandGetNextDeviceID :
        if (receiveLen != 2) {
            return false;
        }
        receiveData[0] = 0;
        receiveData[1] = 0;
        return true;
    case BroadcastCommandSetAddress:
        if (sendLen != 3 || receiveLen != 0) {
            return false;
        }
        _controllerAddress = sendData[2];
        return true;
    case BroadcastCommandGetAddress:
        if (sendLen != 2 || receiveLen != 1) {
            return false;
        }
        receiveData[0] = _controllerAddress;
        return true;
    case BroadcastCommandGetDeviceType:
        if (sendLen != 2 or receiveLen != 31) {
            return false;
        }
        strcpy((char*)receiveData, "co.modulo.controller");
        return true;
    case BroadcastCommandGetDeviceVersion:
        if (sendLen != 2 or receiveLen != 2) {
            return false;
        }
        receiveData[0] = 0;
        receiveData[1] = 0;
        return true;
    case BroadcastCommandGetCompanyName:
        if (sendLen != 2 or receiveLen != 31) {
            return false;
        }
        strcpy((char*)receiveData, "Integer Labs");
        return true;
    case BroadcastCommandGetProductName:
        if (sendLen != 2 or receiveLen != 31) {
            return false;
        }
        strcpy((char*)receiveData, "Controller");
        return true;
    case BroadcastCommandSetStatusLED:
        if (sendLen != 3 or receiveLen != 0) {
            return false;
        }
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, sendData[2]);
        return true;
    }
    return false;
}

static bool _handleControllerTransfer(
    uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen)
{
    switch (command) {
        case ControllerFunctionReadTemperatureProbe:
        if (sendLen == 1 and receiveLen == 2) {
            uint16_t temp = ModuloOneWire::ReadOneWireTemp(sendData[0])*10;
            receiveData[0] = temp & 0xFF;
            receiveData[1] = temp >> 8;
            return true;
        }
        return false;
    }
    return false;
}

bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{            
    // Intercept broadcast transfers to deviceID 0, which is the controller
    if (address == BroadcastAddress and sendLen >= 2 and sendData[0] == 0 and sendData[1] == 0) {
        return _handleControllerBroadcastTransfer(command, sendData, sendLen, receiveData, receiveLen);
    } else if (address == _controllerAddress) {
        // Handle controller transfer
        return _handleControllerTransfer(command, sendData, sendLen, receiveData, receiveLen);
    }

    for (int i=0; i < retries; i++) {
        if (_moduloTransfer(address,command,sendData,sendLen,receiveData,receiveLen)) {
            return true;
        }
    }
    return false;
}


void ModuloGlobalReset() {
    moduloTransfer(BroadcastAddress, BroadcastCommandGlobalReset,
                   0, 0, 0, 0, 1);
    _controllerAddress = 0;
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

bool ModuloGetManufacturer(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetCompanyName,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloGetProduct(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetProductName,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloGetDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return moduloTransfer(BroadcastAddress, BroadcastCommandGetDocURL,
                          sendData, 2, (uint8_t*)deviceType, maxLen, 1);
}

bool ModuloSetStatus(uint16_t deviceID, ModuloStatus status) {
    uint8_t sendData[3] = {deviceID & 0xFF, deviceID >> 8, status};
    return moduloTransfer(BroadcastAddress, BroadcastCommandSetStatusLED,
                          sendData, 3, 0, 0);
}
