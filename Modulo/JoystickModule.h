#ifndef JOYSTICK_MODULE_H
#define JOYSTICK_MODULE_H

#include "Module.h"
#include <inttypes.h>

class JoystickModule : public Module {
 public:
 
    /// Use the first Joystick module that's not already in use
    JoystickModule();

    /// Use the Joystick with the provided deviceID
    JoystickModule(uint16_t deviceID);

    /// Return true if the button is currently pressed
    bool getButton();
    float getHPos();
    float getVPos();

    typedef void (EventCallback)(JoystickModule &module);

    void setButtonPressCallback(EventCallback *handler);
    void setButtonReleaseCallback(EventCallback *handler);
    void setPositionChangeCallback(EventCallback *handler);

protected:
    virtual bool _init();
    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);

    void _refreshState();

private:
    bool _buttonState;
    uint8_t _hPos, _vPos;

    EventCallback *_buttonPressCallback;
    EventCallback *_buttonReleaseCallback;
    EventCallback *_positionChangeCallback;

};

#endif
