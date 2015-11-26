#include "KnobModulo.h"
#include "Modulo.h"
#include <math.h>

#define FUNCTION_KNOB_GET_BUTTON 0
#define FUNCTION_KNOB_GET_POSITION 1
#define FUNCTION_KNOB_ADD_POSITION_OFFSET 2
#define FUNCTION_KNOB_SET_COLOR 3
#define EVENT_BUTTON_CHANGED 0
#define EVENT_POSITION_CHANGED 1

KnobModulo::KnobModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.knob", deviceID), _position(0), _buttonState(false),
    _buttonPressCallback(NULL), _buttonReleaseCallback(NULL), _positionChangeCallback(NULL)
{
}

KnobModulo::KnobModulo() :
    BaseModulo("co.modulo.knob"),  _position(0), _buttonState(false),
    _buttonPressCallback(NULL), _buttonReleaseCallback(NULL), _positionChangeCallback(NULL)
{
}


static void HSVToRGB(float h, float s, float v,
              float *r, float *g, float *b) {
    *r = *g = *b = 0;

    // Put h in the range [0, 6)
    h = 6*(h - floor(h));

    float C = v*s;
    float X = C*(1-fabs(fmod(h, 2) - 1));

    switch (int(h)) {
        case 0:
            *r = C;
            *g = X;
            break;
        case 1:
            *r = X;
            *g = C;
            break;
        case 2:
            *g = C;
            *b = X;
            break;
        case 3:
            *g = X;
            *b = C;
            break;
        case 4:
            *r = X;
            *b = C;
            break;
        case 5:
            *r = C;
            *b = X;
            break;
    }

    float m = v-C;

    *r += m;
    *g += m;
    *b += m;
}

bool KnobModulo::setHSV(float h, float s, float v) {
    float r,g,b;
    HSVToRGB(h,s,v,&r,&g,&b);
    setColor(r, g, b);
}

bool KnobModulo::setColor(float r, float g, float b) {
    uint8_t sendData[3] = {r*255, g*255, b*255};
    return _transfer(FUNCTION_KNOB_SET_COLOR, sendData, 3, 0, 0);
}

bool KnobModulo::getButton() {
    return _buttonState;
}

int16_t KnobModulo::getPosition() {
    return _position;
}

int16_t KnobModulo::getAngle() {
    int16_t position = -getPosition();
    position = ((position % 24) + 24) % 24;
    return position*360/24;
}

bool KnobModulo::_init()
{
    uint8_t positionData[2];
    if (_transfer(FUNCTION_KNOB_GET_POSITION, 0, 0, positionData, 2)) {
        _position = positionData[0] | (positionData[1] << 8);
    }

    uint8_t buttonData[1];
    if (_transfer(FUNCTION_KNOB_GET_BUTTON, 0, 0, buttonData, 1)) {
        _buttonState = buttonData[0];
    }

    if (_positionChangeCallback) {
        _positionChangeCallback(*this);
    }
}

void KnobModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {
    if (eventCode == EVENT_BUTTON_CHANGED) {
        bool buttonPressed = eventData >> 8;
        bool buttonReleased = eventData & 0xFF;

        _buttonState |= buttonPressed;
        _buttonState &= !buttonReleased;

        if (buttonPressed and _buttonPressCallback) {
            _buttonPressCallback(*this);
        }
        if (buttonReleased and _buttonReleaseCallback) {
            _buttonReleaseCallback(*this);
        }
    }

    if (eventCode == EVENT_POSITION_CHANGED) {
        _position = eventData;

        if (_positionChangeCallback) {
            _positionChangeCallback(*this);
        }
    }
}


void KnobModulo::setButtonPressCallback(EventCallback *callback) {
    _buttonPressCallback = callback;
}

void KnobModulo::setButtonReleaseCallback(EventCallback *callback) {
    _buttonReleaseCallback = callback;
}

void KnobModulo::setPositionChangeCallback(EventCallback *callback) {
    _positionChangeCallback = callback;
}

