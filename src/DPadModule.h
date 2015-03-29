#ifndef DPAD_MODULE_H
#define DPAD_MODULE_H

#include "Module.h"

/// A directional button pad with 5 buttons. (up, down, left, right, and center)
class DPadModule : public Module {
 public:
    enum Button {
        RIGHT,///< Right Button
        UP, ///< Up Button
        LEFT, ///< Left Button
        DOWN, ///< Down Button
        CENTER ///< Center Button
    };

    /// Use the first DPad module that's not already in use
    DPadModule();

    /// Use the DPad with the provided deviceID
    DPadModule(uint16_t deviceID);

    /// Return true if the specified button is currently pressed
    bool getButton(Button button);

    /// Return true if the specified button is currently pressed
    bool getButton(uint8_t button);

    /// Return the state of all buttons, one in each bit.
    uint8_t getButtons();

    typedef void (ButtonEventCallback)(DPadModule &module, uint8_t button);

    void setButtonPressCallback(ButtonEventCallback *handler);
    void setButtonReleaseCallback(ButtonEventCallback *handler);

protected:
    virtual bool _init();
    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);

    void _refreshState();

private:
    uint8_t _buttonState;
    ButtonEventCallback *_buttonPressCallback;
    ButtonEventCallback *_buttonReleaseCallback;

};

#endif
