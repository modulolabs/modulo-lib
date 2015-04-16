#include "ClockModule.h"
#include "Modulo.h"

#ifdef SPARK
#include "spark_wiring_print.h"
#else
#include "Print.h"
#endif

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
    
    if (_transfer(FUNCTION_GET_TIME, 0, 0,
                        receivedData, 9)) {
        t.second = receivedData[0];
        t.minute = receivedData[1];
        t.hour = receivedData[2];
        t.day = receivedData[3];
        t.weekday = receivedData[4];
        t.month = receivedData[5];
        t.year = receivedData[6] + 2000;
        t.clockSet = receivedData[7];
        t.battLow = receivedData[8];
    }
    // Handle error?

    return t;
}

void ClockModule::setTime(const Time &t) {
    uint8_t sendData[7];
    sendData[0] = t.second;
    sendData[1] = t.minute;
    sendData[2] = t.hour;
    sendData[3] = t.day;
    sendData[4] = t.weekday;
    sendData[5] = t.month;
    sendData[6] = t.year - 2000;

    _transfer(FUNCTION_SET_TIME, sendData, 7, 0, 0);
}
  
float ClockModule::getTemperatureC() {
    uint8_t receiveData[2] = {0,0};
    if (!_transfer(FUNCTION_GET_TEMPERATURE, 0, 0, receiveData, 2)) {
        return 0;
    }
    int16_t tenths = (receiveData[0] | receiveData[1] << 8);
    return tenths/10.0;
}

float ClockModule::getTemperatureF() {
    return getTemperatureC()*1.8 + 32;
}

void ClockModule::Time::printDay(Print &p) {
    const char *dayNames[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    if (weekday < 7) {
        p.print(dayNames[weekday]);
    }
}

void ClockModule::Time::printDate(Print &p) {
    p.print(month);
    p.print("/");
    p.print(day);
    p.print("/");
    p.print(year);
}

void ClockModule::Time::printTime(Print &p) {
    p.print(hour);
    p.print(":");
    p.print(minute);
    p.print(":");
    p.print(second);
}