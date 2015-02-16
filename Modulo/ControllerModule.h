#ifndef CONTROLLER_MODULE_H
#define CONTROLLER_MODULE_H

#include "Module.h"

/// An illuminated Knob and push button
class ControllerModule : public Module {

public:
    ControllerModule();

    float readTemperatureProbe(uint8_t pin);

 private:
};

#endif
