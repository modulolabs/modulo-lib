#ifndef CONTROLLER_MODULE_H
#define CONTROLLER_MODULE_H

#include "Module.h"

/// The controller module. Drives the bus and provides 6 I/O pins.
class ControllerModule : public Module {

public:
    ControllerModule();

    /// Get the temperature in celsius from a 1-wire temperature sensor connected to the specified pin.
    float readTemperatureProbe(uint8_t pin);

 private:
};

#endif
