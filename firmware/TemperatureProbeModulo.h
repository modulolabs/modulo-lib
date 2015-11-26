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

    /// Return whether temperature probe is connected and has a valid value
    bool isValid();

    /// A callback function
    typedef void (TemperatureChangeCallback)(TemperatureProbeModulo &module);

    /// Set the function that should be called when the button is pressed
    void setTemperatureChangeCallback(TemperatureChangeCallback *handler);

private:
    int16_t _temperature; // Temperature in tenths of degrees C
    bool _isValid;
    TemperatureChangeCallback *_temperatureChangeCallback;

    virtual bool _init();
    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);
};


#endif
