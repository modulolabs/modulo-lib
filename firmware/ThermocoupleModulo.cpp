#include "ThermocoupleModulo.h"
#include "Modulo.h"

ThermocoupleModulo::ThermocoupleModulo() :
    BaseModulo("co.modulo.thermocouple")
{
}

ThermocoupleModulo::ThermocoupleModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.thermocouple", deviceID)
{
}

#define FUNCTION_GET_TEMPERATURE 0

const float ThermocoupleModulo::InvalidTemperature = -1000;
  
float ThermocoupleModulo::getTemperatureC() {
    
    uint8_t receiveData[2] = {0,0};
    
    if (!_transfer(FUNCTION_GET_TEMPERATURE, 0, 0,
                        receiveData, 2)) {
        return 0;
        // Handle error?
    }
    int16_t value = receiveData[0] | (receiveData[1] << 8);
    return value/10.0;
}

float ThermocoupleModulo::getTemperatureF() {
    return getTemperatureC()*1.8 + 32;
}
