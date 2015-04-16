#include "DPadModule.h"
#include "Modulo.h"


#define DPAD_FUNCTION_GET_BUTTONS 0

DPadModule::DPadModule() : Module("co.modulo.dpad"), _buttonState(0), _buttonPressCallback(NULL), _buttonReleaseCallback(NULL) {}

DPadModule::DPadModule(uint16_t deviceID) :
    Module("co.modulo.dpad", deviceID), _buttonState(0), _buttonPressCallback(NULL), _buttonReleaseCallback(NULL) {
}


bool DPadModule::getButton(uint8_t button) {
    _init();
    
    return _buttonState & (1 << button);
}

bool DPadModule::getButton(Button button) {
    return getButton((uint8_t)button);
}

uint8_t DPadModule::getButtons() {
    _init();

    return _buttonState;
}

bool DPadModule::_init()
{
    if (Module::_init()) {
        _refreshState();
        return true;
    }
    return false;
}

void DPadModule::_refreshState() {
    _transfer(DPAD_FUNCTION_GET_BUTTONS, 0, 0, &_buttonState, 1);
}

void DPadModule::_processEvent(uint8_t eventCode, uint16_t eventData) {
    uint8_t buttonPresses = eventData >> 8;
    uint8_t buttonReleases = eventData & 0xFF;

    _buttonState |= buttonPresses;
    _buttonState &= ~buttonReleases;

    if (_buttonPressCallback) {
        for (int i=0; i < 5; i++) {
            if (buttonPresses & (1 << i)) {
                _buttonPressCallback(*this, i);
            }
        }
    }

    if (_buttonReleaseCallback) {
        for (int i=0; i < 5; i++) {
            if (buttonReleases & (1 << i)) {
                _buttonReleaseCallback(*this, i);
            }
        }
    }

}


void DPadModule::setButtonPressCallback(ButtonEventCallback *callback) {
    _buttonPressCallback = callback;
}

void DPadModule::setButtonReleaseCallback(ButtonEventCallback *callback) {
    _buttonReleaseCallback = callback;
}

