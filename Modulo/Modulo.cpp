#include <Wire.h>
#include "Modulo.h"
#include "Arduino.h"

ModThermocouple::ModThermocouple() :
    ModBase("co.modulo.thermocouple")
{
}

ModThermocouple::ModThermocouple(uint16_t deviceID) :
    ModBase("co.modulo.thermocouple", deviceID)
{
}

#define FUNCTION_GET_TEMPERATURE_A 0
#define FUNCTION_GET_TEMPERATURE_B 1

const float ModThermocouple::InvalidTemperature = 6553.5;
  
int16_t ModThermocouple::getTemperature() {
    
    uint8_t receiveData[2] = {0,0};
    
    if (!moduloTransfer(getAddress(), FUNCTION_GET_TEMPERATURE_A, 0, 0,
                        receiveData, 2)) {
        return 0;
        // Handle error?
    }
    int16_t value = receiveData[0] | (receiveData[1] << 8);
    return value;
}

#define FUNCTION_GET_TIME 0
#define FUNCTION_SET_TIME 1
#define FUNCTION_GET_TEMPERATURE 2

ModClock::ModClock(uint16_t deviceID) : ModBase("co.modulo.clock", deviceID) {
}

ModClock::ModClock() : ModBase("co.modulo.clock") {
}

ModClock::Time ModClock::getTime() {
    uint8_t receivedData[9];
    Time t;
    
    if (moduloTransfer(getAddress(), FUNCTION_GET_TIME, 0, 0,
                        receivedData, 9)) {
        t.seconds = receivedData[0];
        t.minutes = receivedData[1];
        t.hours = receivedData[2];
        t.days = receivedData[3];
        t.weekdays = receivedData[4];
        t.months = receivedData[5];
        t.years = receivedData[6] + 2000;
        t.clockSet = receivedData[7];
        t.battLow = receivedData[8];
    }
    // Handle error?

    return t;
}

void ModClock::setTime(const Time &t) {
    uint8_t sendData[7];
    sendData[0] = t.seconds;
    sendData[1] = t.minutes;
    sendData[2] = t.hours;
    sendData[3] = t.days;
    sendData[4] = t.weekdays;
    sendData[5] = t.months;
    sendData[6] = t.years - 2000;

    moduloTransfer(getAddress(), FUNCTION_SET_TIME, sendData, 7, 0, 0);
}
  
float ModClock::getTemperature() {
    uint8_t receiveData[2] = {0,0};
    if (!moduloTransfer(getAddress(), FUNCTION_GET_TEMPERATURE, 0, 0, receiveData, 2)) {
        return 0;
    }
    int16_t tenths = (receiveData[0] | receiveData[1] << 8);
    return tenths/10.0;
}


///

ModDPad::ModDPad(uint8_t address) : _address(address) {}

bool ModDPad::sync() {
    uint8_t receivedData[3] = {0,0,0};
    if (!moduloTransfer(_address, 0, 0, 0, receivedData, 3)) {
        return false;
    }
    _state = receivedData[0];
    _pressed = receivedData[1];
    _released = receivedData[2];
    return true;
}

bool ModDPad::getState(int button) {
    return _state & (1 << button);
}

uint8_t ModDPad::getStates() {
    return _state;
}

bool ModDPad::getPressed(int button) {
    return _pressed & (1 << button);
}

uint8_t ModDPad::getPresses() {
    return _pressed;
}

bool ModDPad::getReleased(int button) {
    return _released & (1 << button);
}

uint8_t ModDPad::getReleases() {
    return _released;
}

///

#define FUNCTION_KNOB_GET_STATE 0
#define FUNCTION_KNOB_ADD_POSITION_OFFSET 1
#define FUNCTION_KNOB_SET_COLOR 2

ModKnob::ModKnob(uint8_t address) : _address(address),
                                    _position(0), 
                                    _buttonState(false),
                                    _buttonPressed(false),
                                    _buttonReleased(false)
{
}

bool ModKnob::setColor(float r, float g, float b) {
    uint8_t sendData[3] = {r*255, g*255, b*255};
    return moduloTransfer(_address, FUNCTION_KNOB_SET_COLOR, sendData, 3, 0, 0);
}

bool ModKnob::sync() {
    uint8_t receivedData[5];
    if (!moduloTransfer(_address, FUNCTION_KNOB_GET_STATE, 0, 0, receivedData, 5)) {
        return false;
    }
    
    _position = receivedData[0] | (receivedData[1] << 8);
    _buttonState = receivedData[2];
    _buttonPressed = receivedData[3];
    _buttonReleased = receivedData[4];
}

bool ModKnob::getButtonState() {
    return _buttonState;
}

bool ModKnob::getButtonPressed() {
    return _buttonPressed;
}

bool ModKnob::getButtonReleased() {
    return _buttonReleased;
}

int16_t ModKnob::getPosition() {
    return _position;
}

