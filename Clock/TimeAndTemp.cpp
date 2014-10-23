#include "TimeAndTemp.h"
#include "Modulo.h"

TimeAndTemp::TimeAndTemp(uint8_t address) : _address(address) {}

TimeAndTemp::Time TimeAndTemp::LoadTime() {
    TimeAndTemp::Time t;
    
    ModuloWrite(_address, 8, true); // Load
    ModuloRead(_address, 0, &t.second);
    ModuloRead(_address, 1, &t.minute);
    ModuloRead(_address, 2, &t.hour);
    ModuloRead(_address, 3, &t.day);
    ModuloRead(_address, 4, &t.date);
    ModuloRead(_address, 5, &t.month);
    ModuloRead(_address, 6, &t.year);
    
    return t;
}

void TimeAndTemp::SaveTime(const Time& t) {
    ModuloWrite(_address, 0, t.second);
    ModuloWrite(_address, 1, t.minute);
    ModuloWrite(_address, 2, t.hour);
    ModuloWrite(_address, 3, t.day);
    ModuloWrite(_address, 4, t.date);
    ModuloWrite(_address, 5, t.month);
    ModuloWrite(_address, 6, t.year);
    ModuloWrite(_address, 8, false); // Save
}
    
float TimeAndTemp::GetTemperature() {
    float temp = 0;
    ModuloRead(_address, 7, &temp);
    return temp;
}


