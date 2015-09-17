#include "ControllerModulo.h"

#ifdef SPARK
#include "spark_wiring.h"
#else
#include "Arduino.h"
#endif

#define FUNCTION_SET_PIN_DIRECTION 0
#define FUNCTION_GET_DIGITAL_INPUT 1
#define FUNCTION_SET_DIGITAL_OUTPUT 2
#define FUNCTION_SET_PWM_OUTPUT 3
#define FUNCTION_GET_ANALOG_INPUT 4
#define FUNCTION_READ_TEMPERATURE_PROBE 5

ControllerModulo::ControllerModulo() :
    BaseModulo("co.modulo.controller", 0 /*deviceID*/)
{
}

void ControllerModulo::setPinDirection(uint8_t pin, bool output, bool pullup)
{
    uint8_t data = (pin << 2) | (pullup << 1) | output;
    _transfer(FUNCTION_SET_PIN_DIRECTION, &data, 1, 0, 0);
}

bool ControllerModulo::getDigitalInput(uint8_t pin) {
    uint8_t receiveData = 0;
    if (!_transfer(FUNCTION_GET_DIGITAL_INPUT, &pin, 1, &receiveData, 1)) {
        return false;
    }
    return receiveData & (1 << pin);
}


void ControllerModulo::setDigitalOutput(uint8_t pin, bool value) {
    uint8_t data = (pin << 1) | value;
    _transfer(FUNCTION_SET_DIGITAL_OUTPUT, &data, 1, 0, 0);
}


void ControllerModulo::setPWMOutput(uint8_t pin, float value) {
    uint8_t intValue = 255*max(0, min(1, value));
    uint8_t data[] = {pin, intValue};
    _transfer(FUNCTION_SET_PWM_OUTPUT, data, 2, 0, 0);
}


float ControllerModulo::getAnalogInput(uint8_t pin) {
    uint8_t receiveData[] = {0,0};
    if (!_transfer(FUNCTION_GET_ANALOG_INPUT, &pin, 1, receiveData, 2)) {
        return 0.0;
    }
    return (receiveData[0] | receiveData[1] << 8)/1023.0;
}


