#include <Wire.h>
#include "Modulo.h"
#include "Arduino.h"
#include "MainController.h"

extern "C" {
    #include <utility/twi.h>
}

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
#define BroadcastCommandGetEvent 10
#define BroadcastCommandSetStatusLED 11
#define BroadcastCommandClearEvent 12

#define ControllerFunctionReadTemperatureProbe 0

static bool _initialized = false;
_MainController _mainController;

void ModuloSetup(bool highBitRate) {
    if (_initialized) {
        return;
    }

    _initialized = true;

    twi_init();
    if (highBitRate) {
        TWBR = 6;
        TWSR &= ~(3);
    }
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


bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{            
    // Intercept broadcast transfers to deviceID 0, which is the controller
    if (address == BroadcastAddress and sendLen >= 2 and sendData[0] == 0 and sendData[1] == 0) {
        return _mainController.processBroadcastTransfer(command, sendData, sendLen, receiveData, receiveLen);
    } else if (address == _mainController.getAddress()) {
        // Handle controller transfer
        return _mainController.processTransfer(command, sendData, sendLen, receiveData, receiveLen);
    }

    for (int i=0; i < retries; i++) {
        if (_moduloTransfer(address,command,sendData,sendLen,receiveData,receiveLen)) {
            return true;
        }
    }
    return false;
}

void ModuloLoop() {
    _mainController.loop();

    uint8_t event[5];
    if (moduloTransfer(BroadcastAddress, BroadcastCommandGetEvent, 0, 0, event, 5)) {

        moduloTransfer(BroadcastAddress, BroadcastCommandClearEvent, event, 5, 0, 0);
        uint8_t eventCode = event[0];
        uint16_t deviceID = event[1] | (event[2] << 8);
        uint16_t eventData = event[3] | (event[4] << 8);

        Module *m = Module::findByDeviceID(deviceID);
        if (m) {
            m->_processEvent(eventCode, eventData);
        }
  

   
    }
}

void ModuloGlobalReset() {
    moduloTransfer(BroadcastAddress, BroadcastCommandGlobalReset,
                   0, 0, 0, 0, 1);
    Module::_globalReset();
    _mainController.globalReset();
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
