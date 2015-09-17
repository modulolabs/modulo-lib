#ifndef CONTROLLER_MODULE_H
#define CONTROLLER_MODULE_H

#include "Module.h"

/// The controller module. Drives the bus and provides 6 I/O pins.
class ControllerModulo : public ModuloBase {

public:
    ControllerModulo();

    /// Configure the specified \a pin to behave as an input or an output.
    /// If configured as an input, also either enable or disable the pullup
    /// resistor on the specified pin.
    ///
    /// If the pullup resistor is disabled (the default) and a pin is
    /// not connected, reading from the pin may not return a consistent
    /// value. Enabling the pull-up causes the pin to return true when
    /// disconnected.
    void setPinDirection(uint8_t pin, bool output, bool pullup=true);

    /// Return the value from a specified pin, either HIGH (true) or LOW (false).
    bool getDigitalInput(uint8_t pin);

    /// Set the output value of the specified \a pin. The voltage on the
    /// pin will be driven to 5V (true) or 0V (false). The pin must be
    /// configured as an output with setPinDirection.
    void setDigitalOutput(uint8_t pin, bool value);

    /// Enable pulse width modulation on the specified \a pin. The
    /// provided value represents the precentage of time that the
    /// output will be high.
    void setPWMOutput(uint8_t pin, float value);

    /// Read the analog voltage on the specified pin. This will return a
    /// value between 0 and 1 representing a voltage between 0 and 5V.
    float getAnalogInput(uint8_t pin);
};

#endif
