#include "ThermocoupleModule.h"
#include "Modulo.h"

ThermocoupleModule::ThermocoupleModule() :
    Module("co.modulo.thermocouple")
{
}

ThermocoupleModule::ThermocoupleModule(uint16_t deviceID) :
    Module("co.modulo.thermocouple", deviceID)
{
}

#define FUNCTION_GET_TEMPERATURE 0

const float ThermocoupleModule::InvalidTemperature = -1000;
  
float ThermocoupleModule::getTemperatureC() {
    
    uint8_t receiveData[2] = {0,0};
    
    if (!moduloTransfer(getAddress(), FUNCTION_GET_TEMPERATURE, 0, 0,
                        receiveData, 2)) {
        return 0;
        // Handle error?
    }
    int16_t value = receiveData[0] | (receiveData[1] << 8);
    return value/10.0;
}

float ThermocoupleModule::getTemperatureF() {
    return getTemperatureC()*1.8 + 32;
}
