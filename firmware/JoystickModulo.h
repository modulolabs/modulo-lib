#ifndef JOYSTICK_MODULE_H
#define JOYSTICK_MODULE_H

#include "BaseModulo.h"
#include <inttypes.h>

// A thumb joystick with push button
class JoystickModulo : public BaseModulo {
 public:
 
    /// Use the first JoystickModulo module that's not already in use
    JoystickModulo();

    /// Use the JoystickModulo with the provided deviceID
    JoystickModulo(uint16_t deviceID);

    /// Return true if the button is currently pressed
    bool getButton();
    float getHPos();
    float getVPos();

    /// A callback function
    typedef void (EventCallback)(JoystickModulo &module);

    /// Set the function that should be called when the button is pressed
    void setButtonPressCallback(EventCallback *handler);

    /// Set the function that should be called when the button is released
    void setButtonReleaseCallback(EventCallback *handler);

    /// Set the function that should be called when the position changes
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
