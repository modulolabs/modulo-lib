#include "ControllerModule.h"
#include "Modulo.h"

#define FUNCTION_READ_TEMPERATURE_PROBE 0


ControllerModule::ControllerModule() :
    Module("co.modulo.controller", 0 /*deviceID*/)
{
}

float ControllerModule::readTemperatureProbe(uint8_t pin)
{
    uint8_t receiveData[2] = {0,0};
    if (!moduloTransfer(getAddress(), FUNCTION_READ_TEMPERATURE_PROBE, &pin, 1, receiveData, 2)) {
        return 0;
    }
    int16_t tenths = (receiveData[0] | receiveData[1] << 8);
    return tenths/10.0;
}
