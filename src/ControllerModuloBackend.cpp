#include "ControllerModuloBackend.h"

#ifdef SPARK
#include <spark_wiring.h>
#else
#include "Arduino.h"
#endif

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

_ControllerModuloBackend::_ControllerModuloBackend() :
    _address(0), _status(ModuloStatusOff) {
}

void _ControllerModuloBackend::loop() {

}

uint8_t _ControllerModuloBackend::getAddress() {
    return _address;
}

void _ControllerModuloBackend::globalReset() {
    _status = ModuloStatusOff;
    _address = 0;
}


bool _ControllerModuloBackend::processTransfer(
    uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, bool receiveString)
{
    switch (command) {
        case FUNCTION_SET_PIN_DIRECTION:
            if (sendLen == 1 and receiveLen == 0) {
                bool output = sendData[0] & 1;
                bool pullup = sendData[0] & 2;
                uint8_t pin = sendData[0] >> 2;
                if (output) {
                    pinMode(pin, OUTPUT);
                } else if (pullup) {
                    pinMode(pin, INPUT_PULLUP);
                } else {
                    pinMode(pin, INPUT);
                }
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
            if (sendLen == 1 and receiveLen == 2) {
                int val = analogRead(sendData[0]);
                receiveData[0] = val & 0xFF;
                receiveData[1] = val >> 8;
                return true;
            }
            return false;
        return false;
    }
    return false;
}


bool _ControllerModuloBackend::processBroadcastTransfer(
    uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, bool receiveString)
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
        if (sendLen != 2 or !receiveString) {
            return false;
        }
        strncpy((char*)receiveData, "co.modulo.controller", receiveLen);
        return true;
    case BroadcastCommandGetDeviceVersion:
        if (sendLen != 2 or receiveLen != 2) {
            return false;
        }
        receiveData[0] = 0;
        receiveData[1] = 0;
        return true;
    case BroadcastCommandGetCompanyName:
        if (sendLen != 2 or !receiveString) {
            return false;
        }
        strncpy((char*)receiveData, "Modulo Labs", receiveLen);
        return true;
    case BroadcastCommandGetProductName:
        if (sendLen != 2 or !receiveString) {
            return false;
        }
        strncpy((char*)receiveData, "Controller", receiveLen);
        return true;
    case BroadcastCommandSetStatusLED:
        return false;
    }
    return false;
}
