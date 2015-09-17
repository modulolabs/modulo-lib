#ifndef THERMOCOUPLE_MODULE_H
#define THERMOCOUPLE_MODULE_H

#include "Module.h"

/// A module for reading the temperature from a thermocouple
class ThermocoupleModulo : public ModuloBase {
 public:
    explicit ThermocoupleModulo(uint16_t deviceID);
    ThermocoupleModulo();
  
    /// Return the temperature of the thermocouple in celsius
    /// If no thermocouple is connected, returns InvalidTemperature.
    float getTemperatureC();

    /// Return the temperature of the thermocouple in fahrenheit
    /// If no thermocouple is connected, returns InvalidTemperature.
    float getTemperatureF();

    static const float InvalidTemperature;
};

#endif
