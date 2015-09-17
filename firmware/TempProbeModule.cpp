#include "TempProbeModule.h"
#include "Modulo.h"

TempProbeModule::TempProbeModule() :
    Module("co.modulo.tempprobe")
{
}

TempProbeModule::TempProbeModule(uint16_t deviceID) :
    Module("co.modulo.tempprobe", deviceID)
{
}

#define FUNCTION_GET_TEMPERATURE 0

const float TempProbeModule::InvalidTemperature = -1000;

float TempProbeModule::getTemperatureC() {

    uint8_t receiveData[2] = {0,0};

    if (!_transfer(FUNCTION_GET_TEMPERATURE, 0, 0,
                        receiveData, 2)) {
        return 0;
        // Handle error?
    }
    int16_t value = receiveData[0] | (receiveData[1] << 8);
    return value/10.0;
}

float TempProbeModule::getTemperatureF() {
    return getTemperatureC()*1.8 + 32;
}
