#ifndef THERMOCOUPLE_MODULE_H
#define THERMOCOUPLE_MODULE_H

#include "Module.h"

/// A module for reading the temperature from a thermocouple
class ThermocoupleModule : public Module {
 public:
    explicit ThermocoupleModule(uint16_t deviceID);
    ThermocoupleModule();
  
    int16_t getTemperature();
    
    static const float InvalidTemperature;
};

#endif
