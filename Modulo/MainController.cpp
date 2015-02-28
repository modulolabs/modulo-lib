#include "MainController.h"
#include "DallasTemperature.h"

#define FUNCTION_SET_PIN_DIRECTION 0
#define FUNCTION_GET_DIGITAL_INPUT 1
#define FUNCTION_SET_DIGITAL_OUTPUT 2
#define FUNCTION_SET_PWM_OUTPUT 3
#define FUNCTION_GET_ANALOG_INPUT 4
#define FUNCTION_READ_TEMPERATURE_PROBE 5


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

_MainController::_MainController() :
    _address(0), _status(ModuloStatusOff) {
}

void _MainController::loop() {
    if (_status == ModuloStatusBlinking) {
        digitalWrite(LED_BUILTIN, millis() % 500 > 250);
    } else {
        digitalWrite(LED_BUILTIN, _status == ModuloStatusOn);
    }
}

uint8_t _MainController::getAddress() {
    return _address;
}

void _MainController::globalReset() {
    _status = ModuloStatusOff;
    _address = 0;
}


bool _MainController::processTransfer(
    uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen)
{
    switch (command) {
        case FUNCTION_SET_PIN_DIRECTION:
            if (sendLen == 1 and receiveLen == 0) {
                uint8_t pin = sendData[0] >> 2;
                bool mode = sendData[0] & 3;
                pinMode(pin, mode);
                return true;
            }
            return false;
        case FUNCTION_GET_DIGITAL_INPUT:
            if (sendLen == 1 and receiveLen == 1) {
                receiveData[0] = digitalRead(sendData[0]);
                return true;
            }
            return false;
        case FUNCTION_SET_DIGITAL_OUTPUT:
            if (sendLen == 1 and receiveLen == 0) {
                digitalWrite(sendData[0] >> 1, sendData[0] & 1);
            }
            return false;
        case FUNCTION_SET_PWM_OUTPUT:
            if (sendLen == 2 and receiveLen == 0) {
                analogWrite(sendData[0], sendData[1]);
                return true;
            }
            return false;
        case FUNCTION_GET_ANALOG_INPUT:
            if (sendLen == 1 and receiveLen == 1) {
                receiveData[0] = analogRead(sendData[0]);
                return true;
            }
            return false;
        case FUNCTION_READ_TEMPERATURE_PROBE:
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


bool _MainController::processBroadcastTransfer(
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
        _address = sendData[2];
        return true;
    case BroadcastCommandGetAddress:
        if (sendLen != 2 || receiveLen != 1) {
            return false;
        }
        receiveData[0] = _address;
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
        _status = (ModuloStatus)sendData[2];
        pinMode(LED_BUILTIN, OUTPUT);
        return true;
    }
    return false;
}
