#include "ClockModule.h"
#include "Modulo.h"

#define FUNCTION_GET_TIME 0
#define FUNCTION_SET_TIME 1
#define FUNCTION_GET_TEMPERATURE 2

ClockModule::ClockModule(uint16_t deviceID) : Module("co.modulo.clock", deviceID) {
}

ClockModule::ClockModule() : Module("co.modulo.clock") {
}

ClockModule::Time ClockModule::getTime() {
    uint8_t receivedData[9];
    Time t;
    
    if (moduloTransfer(getAddress(), FUNCTION_GET_TIME, 0, 0,
                        receivedData, 9)) {
        t.seconds = receivedData[0];
        t.minutes = receivedData[1];
        t.hours = receivedData[2];
        t.days = receivedData[3];
        t.weekdays = receivedData[4];
        t.months = receivedData[5];
        t.years = receivedData[6] + 2000;
        t.clockSet = receivedData[7];
        t.battLow = receivedData[8];
    }
    // Handle error?

    return t;
}

void ClockModule::setTime(const Time &t) {
    uint8_t sendData[7];
    sendData[0] = t.seconds;
    sendData[1] = t.minutes;
    sendData[2] = t.hours;
    sendData[3] = t.days;
    sendData[4] = t.weekdays;
    sendData[5] = t.months;
    sendData[6] = t.years - 2000;

    moduloTransfer(getAddress(), FUNCTION_SET_TIME, sendData, 7, 0, 0);
}
  
float ClockModule::getTemperature() {
    uint8_t receiveData[2] = {0,0};
    if (!moduloTransfer(getAddress(), FUNCTION_GET_TEMPERATURE, 0, 0, receiveData, 2)) {
        return 0;
    }
    int16_t tenths = (receiveData[0] | receiveData[1] << 8);
    return tenths/10.0;
}

