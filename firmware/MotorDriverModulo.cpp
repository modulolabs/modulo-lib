#include "MotorDriverModulo.h"
#include "Modulo.h"
#include "Arduino.h"

static float _constrain(float v, float min, float max) {
    if (v < min) {
        return min;
    }
    if (v > max) {
        return max;
    }
    return v;
}


static const uint8_t _FunctionSetValue = 0;
static const uint8_t _FunctionSetEnabled = 1;
static const uint8_t _FunctionSetFrequency = 2;
static const uint8_t _FunctionSetCurrentLimit = 3;

MotorDriverModulo::MotorDriverModulo() : BaseModulo("co.modulo.motor") {
}

MotorDriverModulo::MotorDriverModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.motor", deviceID) {
}

void MotorDriverModulo::setChannel(uint8_t channel, float amount) {
    uint16_t intValue = _constrain(amount, 0, 1)*0xFFFF;
    uint8_t data[] = {channel, intValue & 0xFF, intValue >> 8};
    _transfer(_FunctionSetValue, data, 3, 0, 0);
}

void MotorDriverModulo::setMotorA(float value) {
    if (value > 0) {
        setChannel(0, 1);
        setChannel(1, 1-value);
    } else {
        setChannel(0, 1+value);
        setChannel(1, 1);
    }
}

void MotorDriverModulo::setMotorB(float value) {
    if (value > 0) {
        setChannel(2, value);
        setChannel(3, 0);
    } else {
        setChannel(2, 0);
        setChannel(3, -value);
    }
}

void MotorDriverModulo::setEnable(bool enable) {
    uint8_t dataToSend[] = {enable};
    _transfer(_FunctionSetEnabled, dataToSend, 1, 0, 0);
}

void MotorDriverModulo::setCurrentLimit(float limit) {
    limit = constrain(limit, 0.0, 1.0);
    uint8_t dataToSend[] = {limit*64};
    _transfer(_FunctionSetCurrentLimit, dataToSend, 1, 0, 0);
}