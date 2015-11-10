#include "Modulo.h"
#include "ControllerModuloBackend.h"
#include "ModuloWiring.h"

#define BroadcastAddress 9

#define BroadcastCommandGlobalReset 0
#define BroadcastCommandGetNextDeviceID 1
#define BroadcastCommandSetAddress 2
#define BroadcastCommandGetAddress 3
#define BroadcastCommandGetDeviceType 4
#define BroadcastCommandGetDeviceVersion 5
#define BroadcastCommandGetEvent 6
#define BroadcastCommandClearEvent 7
#define BroadcastCommandSetStatusLED 8

#if MODULO_CUSTOM_WIRE
#include "ModuloTWI.h"

#define TWI_INIT() modulo_twi_init()
#define TWI_BEGIN_WRITE(address) modulo_twi_beginWrite(address)
#define TWI_WRITE(data) modulo_twi_write(data)
#define TWI_END_WRITE(sendStop) modulo_twi_endWrite(true /*wait*/, sendStop/*sendStop*/)
#define TWI_REQUEST_FROM(address, len) modulo_twi_readFrom(address, len, true /*sendStop*/)
#define TWI_READ() modulo_twi_read()
#define TWI_AVAILABLE() modulo_twi_available()
#else

#ifdef SPARK
#include "spark_wiring_i2c.h"
#else
#include "Wire.h"
#endif

#define TWI_INIT() Wire.begin()
#define TWI_BEGIN_WRITE(address) Wire.beginTransmission(address)
#define TWI_WRITE(data) Wire.write(data)
#define TWI_END_WRITE(sendStop) Wire.endTransmission(sendStop)
#define TWI_REQUEST_FROM(address, len) Wire.requestFrom((int)address, (int)len)
#define TWI_READ() Wire.read()
#define TWI_AVAILABLE() Wire.available()
#endif

static bool _initialized = false;
_ControllerModuloBackend _mainController;
_Modulo Modulo;

void _Modulo::setup() {
    if (_initialized) {
        return;
    }

    _initialized = true;

    // Wait until at least 100ms after startup
    // so that connected devices can initialize
    unsigned long t = millis();
    if (t < 100) {
        delay(100-t);
    }

    int i2cSpeed = 600000;

#ifdef SPARK
    // The spark firmware requires that the speed be set before calling begin
    Wire.setSpeed(i2cSpeed);
#endif

    TWI_INIT();

#ifdef ARDUINO
    // The arduino firmware requires that the speed be set after calling begin
    Wire.setClock(1000000);
#endif

    globalReset();
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
    uint8_t *receiveData, uint8_t receiveLen, bool receiveString)
{
    // Star the transmit CRC with the address in the upper 7 bits
    uint8_t crc =  _crc8_ccitt_update(0, address);

    TWI_BEGIN_WRITE(address);

    // Send the command and length
    TWI_WRITE(command);
    TWI_WRITE(sendLen);
    crc = _crc8_ccitt_update(crc, command);
    crc = _crc8_ccitt_update(crc, sendLen);

    // Send the data
    for (int i=0; i < sendLen; i++) {
        TWI_WRITE(sendData[i]);
        crc = _crc8_ccitt_update(crc, sendData[i]);
    }

    // Send the CRC and end the transmission
    TWI_WRITE(crc);

    if (TWI_END_WRITE(receiveLen == 0) != 0) {
        return false;
    }

    if (receiveLen == 0) {
        return true;
    }

    while (TWI_AVAILABLE()) {
        TWI_READ();
    }

    // Request receiveLen data bytes plus 1 CRC byte.
    if (TWI_REQUEST_FROM((int)address, (int)receiveLen+1) != receiveLen+1) {
        return false;
    }

    // Start the CRC with the I2C address byte (address in upper 7, 1 in lsb)
    crc = _crc8_ccitt_update(0, address);

    // Receive the data
    for (int i=0; i < receiveLen; i++) {
        receiveData[i] = TWI_READ();

        // XXX: Hack to detect the end of variable length strings.
        if (i > 0 and receiveLen == 31 and
            receiveData[i-1] == 0 and receiveData[i] == crc) {
            return true;
        }
        crc = _crc8_ccitt_update(crc, receiveData[i]);
    }

    // Receive the CRC.
    uint8_t receivedCRC = TWI_READ();

    // Check the CRC.
    return (crc == receivedCRC);
}


bool _Modulo::transfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, bool receiveString)
{
    // Ensure that the twi bus has been initialised and a reset has been performed
    setup();

    // Intercept broadcast transfers to deviceID 0, which is the controller
    // XXX: event messages don't start with the deviceID
/*
    if (address == BroadcastAddress and sendLen >= 2 and sendData[0] == 0 and sendData[1] == 0) {
        return _mainController.processBroadcastTransfer(command, sendData, sendLen, receiveData, receiveLen, receiveString);
    } else if (address == _mainController.getAddress()) {
        // Handle controller transfer
        return _mainController.processTransfer(command, sendData, sendLen, receiveData, receiveLen, receiveString);
    }
*/

    return _moduloTransfer(address, command, sendData, sendLen, receiveData, receiveLen, receiveString);
}

void _Modulo::loop() {

    _mainController.loop();

    BaseModulo::loop();

    uint8_t event[5];
    if (transfer(BroadcastAddress, BroadcastCommandGetEvent, 0, 0, event, 5)) {

        transfer(BroadcastAddress, BroadcastCommandClearEvent, event, 5, 0, 0);

        uint8_t eventCode = event[0];
        uint16_t deviceID = event[1] | (event[2] << 8);
        uint16_t eventData = event[3] | (event[4] << 8);

        BaseModulo *m = BaseModulo::findByDeviceID(deviceID);
        if (m) {
            m->_processEvent(eventCode, eventData);
        }
    }
}

void _Modulo::globalReset() {
    transfer(BroadcastAddress, BroadcastCommandGlobalReset,
                   0, 0, 0, 0);
    BaseModulo::_globalReset();
    _mainController.globalReset();
}

uint16_t _Modulo::getNextDeviceID(uint16_t lastDeviceID) {
    if (lastDeviceID == 0xFFFF) {
        return 0xFFFF;
    }
    uint16_t nextDeviceID = lastDeviceID +1;

    uint8_t sendData[2] = {nextDeviceID & 0xFF, nextDeviceID >> 8 };
    uint8_t receiveData[2] = {0xFF,0xFF};
    if (!transfer(BroadcastAddress, BroadcastCommandGetNextDeviceID,
                        sendData, 2, receiveData, 2)) {
        return 0xFFFF;
    }
    return receiveData[1] | (receiveData[0] << 8);
}

bool _Modulo::setAddress(uint16_t deviceID, uint8_t address) {
    uint8_t sendData[3] = {deviceID & 0xFF, deviceID >> 8, address};
    return transfer(BroadcastAddress, BroadcastCommandSetAddress,
                          sendData, 3, 0, 0);
}

uint8_t _Modulo::getAddress(uint16_t deviceID) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    uint8_t address = 0;
    if (transfer(BroadcastAddress, BroadcastCommandGetAddress,
                       sendData, 2, &address, 1)){
        return address;
    }
    return 0;
}

bool _Modulo::getDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen) {
    uint8_t sendData[2] = {deviceID & 0xFF, deviceID >> 8};
    return transfer(BroadcastAddress, BroadcastCommandGetDeviceType,
                          sendData, 2, (uint8_t*)deviceType, maxLen, true);
}

bool _Modulo::setStatus(uint16_t deviceID, ModuloStatus status) {
    uint8_t sendData[3] = {deviceID & 0xFF, deviceID >> 8, status};
    return transfer(BroadcastAddress, BroadcastCommandSetStatusLED,
                          sendData, 3, 0, 0);
}

