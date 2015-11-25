#include "TemperatureProbeModulo.h"
#include "Modulo.h"
#include "ModuloWiring.h"

#define FUNCTION_GET_TEMPERATURE 0
#define EVENT_TEMPERATURE_CHANGED 0

TemperatureProbeModulo::TemperatureProbeModulo() :
    BaseModulo("co.modulo.tempprobe"), _temperatureChangeCallback(NULL),
    _isValid(false)
{
}

TemperatureProbeModulo::TemperatureProbeModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.tempprobe", deviceID), _temperatureChangeCallback(NULL),
    _isValid(false)
{
}

float TemperatureProbeModulo::getTemperatureC() {
    return _temperature/10.0;
}

float TemperatureProbeModulo::getTemperatureF() {
    return getTemperatureC()*1.8 + 32;
}

bool TemperatureProbeModulo::isValid() {
    return _isValid;
}


bool TemperatureProbeModulo::_init()
{
    if (BaseModulo::_init()) {
        _refreshState();
        return true;
    }
    return false;
}

void TemperatureProbeModulo::_refreshState() {
    uint8_t receiveData[2] = {0,0};

    if (_transfer(FUNCTION_GET_TEMPERATURE, 0, 0, receiveData, 2)) {

        _temperature = static_cast<int16_t>(receiveData[0] | (receiveData[1] << 8));
        _isValid = true;
    } else {
        _temperature = 0;
        _isValid = false;
    }
}

void TemperatureProbeModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {
    if (eventCode == EVENT_TEMPERATURE_CHANGED) {
        _temperature = eventData;
        _isValid = true;

        if (_temperatureChangeCallback) {
            _temperatureChangeCallback(*this);
        }
    }
}


void TemperatureProbeModulo::setTemperatureChangeCallback(TemperatureChangeCallback *callback) {
    _temperatureChangeCallback = callback;
}
