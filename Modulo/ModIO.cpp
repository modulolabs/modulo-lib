#include "ModIO.h"
#include "Modulo.h"
#include <math.h>

namespace {
    enum GPIOCommands {
        FUNCTION_GET_DIGITAL_INPUT,
        FUNCTION_GET_DIGITAL_INPUTS,
        FUNCTION_GET_ANALOG_INPUT,
        FUNCTION_SET_DIGITAL_OUTPUT,
        FUNCTION_SET_DIGITAL_OUTPUTS,
        FUNCTION_SET_PWM_OUTPUT,
        FUNCTION_SET_PULLUP,
        FUNCTION_SET_DEBOUNCE,
        FUNCTION_SET_PWM_FREQUENCY
    };
};


ModIO::ModIO(uint8_t address) : _address(address) {
}

bool ModIO::getDigitalInput(uint8_t pin) {
    uint8_t value = false;

    if (!moduloTransfer(_address, FUNCTION_GET_DIGITAL_INPUT, &pin, sizeof(pin),
                        &value, sizeof(value))) {
        // Handle error?
    }
    return value;
}
 

uint8_t ModIO::getDigitalInputs() {
    uint8_t value = 0;

    if (!moduloTransfer(_address, FUNCTION_GET_DIGITAL_INPUTS, 0, 0,
                        &value, sizeof(value))) {
        // Handle error?
    }
    return value;
}

 
float ModIO::getAnalogInput(uint8_t pin, AnalogReference ref) {
    uint8_t sendData[] = {pin, ref};
    uint8_t receiveData[2];

    if (!moduloTransfer(_address, FUNCTION_GET_ANALOG_INPUT, sendData, sizeof(sendData),
                        receiveData, sizeof(receiveData))) {
        // Handle error?
    }
    return ((receiveData[1] << 8 ) | receiveData[0]) / 1023.0;
}

void ModIO::setDigitalOutput(uint8_t pin, bool value) {
    uint8_t sendData[] = {pin, value};
    
    if (!moduloTransfer(_address, FUNCTION_SET_DIGITAL_OUTPUT, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

bool ModIO::setDigitalOutputs(uint8_t values) {
    return moduloTransfer(_address, FUNCTION_SET_DIGITAL_OUTPUTS, &values, 1, 0, 0);
}

void ModIO::setPWMOutput(uint8_t pin, float value) {
    uint16_t v = 65535.0 * fmax(0.0, fmin(1.0, value));
    
    uint8_t sendData[] = {pin, v & 0xFF, v >> 8};    

    if (!moduloTransfer(_address, FUNCTION_SET_PWM_OUTPUT, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

void ModIO::setPullup(uint8_t pin, bool enable) {
    uint8_t sendData[] = {pin, enable};

    if (!moduloTransfer(_address, FUNCTION_SET_PULLUP, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}
  
void ModIO::setDebounce(uint8_t pin, bool enable) {
    uint8_t sendData[] = {pin, enable};

    if (!moduloTransfer(_address, FUNCTION_SET_DEBOUNCE, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

void ModIO::setPWMFrequency(uint8_t pin, uint16_t value) {
    uint8_t sendData[] = {pin, value & 0xFF, value >> 8};

    if (!moduloTransfer(_address, FUNCTION_SET_PWM_FREQUENCY, sendData, sizeof(sendData), 0, 0)) {
        // Handle error?
    }
}

