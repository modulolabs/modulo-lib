#include "BlankSlateModulo.h"
#include <math.h>

namespace {
    enum GPIOCommands {
        FUNCTION_GET_DIGITAL_INPUT,
        FUNCTION_GET_DIGITAL_INPUTS,
        FUNCTION_GET_ANALOG_INPUT,
        FUNCTION_SET_DATA_DIRECTION,
        FUNCTION_SET_DATA_DIRECTIONS,
        FUNCTION_SET_DIGITAL_OUTPUT,
        FUNCTION_SET_DIGITAL_OUTPUTS,
        FUNCTION_SET_PWM_OUTPUT,
        FUNCTION_SET_PULLUP,
        FUNCTION_SET_PULLUPS,
        FUNCTION_SET_DEBOUNCE,
        FUNCTION_SET_DEBOUNCES,
        FUNCTION_SET_PWM_FREQUENCY
    };
};

BlankSlateModulo::BlankSlateModulo() : BaseModulo("co.modulo.io")
{
}

BlankSlateModulo::BlankSlateModulo(uint16_t deviceID) : BaseModulo("co.modulo.io", deviceID)
{
}

bool BlankSlateModulo::getDigitalInput(uint8_t pin) {
    uint8_t value = false;

    if (!_transfer(FUNCTION_GET_DIGITAL_INPUT, &pin, sizeof(pin),
                        &value, sizeof(value))) {
        return false;
    }
    return value;
}


uint8_t BlankSlateModulo::getDigitalInputs() {
    uint8_t value = 0;

    if (!_transfer(FUNCTION_GET_DIGITAL_INPUTS, 0, 0,
                        &value, sizeof(value))) {
        return false;
    }
    return value;
}


float BlankSlateModulo::getAnalogInput(uint8_t pin, AnalogReference ref) {
    uint8_t sendData[] = {pin, ref};
    uint8_t receiveData[2];

    if (!_transfer(FUNCTION_GET_ANALOG_INPUT, sendData, sizeof(sendData),
                        receiveData, sizeof(receiveData))) {
        return 0;
    }
    return ((receiveData[1] << 8 ) | receiveData[0]) / 1023.0;
}


void BlankSlateModulo::setDirection(uint8_t pin, bool output) {
    uint8_t sendData[] = {pin, output};

    if (!_transfer(FUNCTION_SET_DATA_DIRECTION, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}


void BlankSlateModulo::setDirections(uint8_t outputs) {
    _transfer(FUNCTION_SET_DATA_DIRECTIONS, &outputs, 1, 0, 0);
}


void BlankSlateModulo::setDigitalOutput(uint8_t pin, bool value) {
    uint8_t sendData[] = {pin, value};

    if (!_transfer(FUNCTION_SET_DIGITAL_OUTPUT, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

bool BlankSlateModulo::setDigitalOutputs(uint8_t values) {
    return _transfer(FUNCTION_SET_DIGITAL_OUTPUTS, &values, 1, 0, 0);
}

void BlankSlateModulo::setPWMValue(uint8_t pin, float value) {
    uint16_t v = 65535.0 * fmax(0.0, fmin(1.0, value));

    uint8_t sendData[] = {pin, v & 0xFF, v >> 8};

    if (!_transfer(FUNCTION_SET_PWM_OUTPUT, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

void BlankSlateModulo::setPullup(uint8_t pin, bool enable) {
    uint8_t sendData[] = {pin, enable};

    if (!_transfer(FUNCTION_SET_PULLUP, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

void BlankSlateModulo::setDebounce(uint8_t pin, bool enable) {
    uint8_t sendData[] = {pin, enable};

    if (!_transfer(FUNCTION_SET_DEBOUNCE, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

void BlankSlateModulo::setPWMFrequency(uint8_t pin, uint16_t value) {
    uint8_t sendData[] = {pin, value & 0xFF, value >> 8};

    if (!_transfer(FUNCTION_SET_PWM_FREQUENCY, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}
