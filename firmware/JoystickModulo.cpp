#include "JoystickModulo.h"
#include "Modulo.h"
#include <stdlib.h>

#define FUNCTION_GET_BUTTON 0
#define FUNCTION_GET_POSITION 1

#define EVENT_BUTTON_CHANGED 0
#define EVENT_POSITION_CHANGED 1

JoystickModulo::JoystickModulo() : BaseModulo("co.modulo.joystick"), _buttonState(0), _hPos(0), _vPos(0),
    _buttonPressCallback(NULL), _buttonReleaseCallback(NULL), _positionChangeCallback(NULL) {
}

JoystickModulo::JoystickModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.joystick", deviceID), _hPos(0), _vPos(0), _buttonState(0),
    _buttonPressCallback(NULL), _buttonReleaseCallback(NULL), _positionChangeCallback(NULL) {
}


bool JoystickModulo::getButton() {
    _init();

    return _buttonState;
}

float JoystickModulo::getHPos() {
    _init();

    return _hPos*2/255.0 - 1;
}

float JoystickModulo::getVPos() {
    _init();

    return _vPos*2/255.0 - 1;
}


bool JoystickModulo::_init()
{
    if (BaseModulo::_init()) {
        _refreshState();
        return true;
    }
    return false;
}

void JoystickModulo::_refreshState() {
    uint8_t buttonState = 0;
    _transfer(FUNCTION_GET_BUTTON, 0, 0, &buttonState, 1);
    _buttonState = buttonState;

    uint8_t posResult[2] = {0,0};
    _transfer(FUNCTION_GET_POSITION, 0, 0, posResult, 2);

    _hPos = posResult[0];
    _vPos = posResult[1];
}

void JoystickModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {

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
        _hPos = eventData >> 8;
        _vPos = eventData & 0xFF;

        if (_positionChangeCallback) {
            _positionChangeCallback(*this);
        }
    }

}


void JoystickModulo::setButtonPressCallback(EventCallback *callback) {
    _buttonPressCallback = callback;
}

void JoystickModulo::setButtonReleaseCallback(EventCallback *callback) {
    _buttonReleaseCallback = callback;
}

void JoystickModulo::setPositionChangeCallback(EventCallback *callback) {
    _positionChangeCallback = callback;
}
