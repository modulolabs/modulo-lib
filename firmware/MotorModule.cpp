#include "MotorModule.h"

#include "Modulo.h"

static float constrain(float v, float min, float max) {
    if (v < min) {
        return min;
    }
    if (v < max) {
        return max;
    }
    return v;
}

static const uint8_t _FunctionSetValue = 0;
static const uint8_t _FunctionGetCurrent = 1;
static const uint8_t _FunctionSetEnabled = 2;
static const uint8_t _FunctionSetFrequency = 3;

MotorDriverModulo::MotorDriverModulo() : ModuloBase("co.modulo.motor") {
}

MotorDriverModulo::MotorDriverModulo(uint16_t deviceID) :
    ModuloBase("co.modulo.motor", deviceID) {
}

void MotorDriverModulo::setChannel(uint8_t channel, float amount) {
    uint16_t intValue = constrain(amount, 0, 1)*0xFFFF;
    uint8_t data[] = {channel, intValue & 0xFF, intValue >> 8};
    _transfer(_FunctionSetValue, data, 3, 0, 0);
}

void MotorDriverModulo::setMotorA(float value) {
    if (value > 0) {
        setChannel(0, value);
        setChannel(1, 0);
    } else {
        setChannel(0, 0);
        setChannel(1, -value);
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

void MotorDriverModulo::setEnableA(bool enable) {
    uint8_t dataToSend[] = {0, enable};
    _transfer(_FunctionSetEnabled, dataToSend, 2, 0, 0);

}

void MotorDriverModulo::setEnableB(bool enable) {
    uint8_t dataToSend[] = {2, enable};
    _transfer(_FunctionSetEnabled, dataToSend, 2, 0, 0);
}

