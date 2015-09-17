#ifndef TEMP_PROBE_MODULE_H
#define TEMP_PROBE_MODULE_H

#include "BaseModulo.h"

/// A module for reading the temperature from a thermocouple
class TemperatureProbeModulo : public BaseModulo {
 public:
    explicit TemperatureProbeModulo(uint16_t deviceID);
    TemperatureProbeModulo();

    /// Return the temperature of the thermocouple in celsius
    /// If no thermocouple is connected, returns InvalidTemperature.
    float getTemperatureC();

    /// Return the temperature of the thermocouple in fahrenheit
    /// If no thermocouple is connected, returns InvalidTemperature.
    float getTemperatureF();

    static const float InvalidTemperature;
};

#endif
