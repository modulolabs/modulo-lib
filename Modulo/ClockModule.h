#ifndef CLOCK_MODULE_H
#define CLOCK_MODULE_H

#include "Module.h"

class Print;

/// Keeps track of the current time and date, even without power. Also senses the ambient air temperature.
class ClockModule : public Module {
 public:
    struct Time {
    Time() : second(0), minute(0), hour(0), day(0), weekday(0), month(0), year(0), clockSet(false), battLow(false) {}
        
        void printDay(Print &p);
        void printDate(Print &p);
        void printTime(Print &p);
        
        uint8_t second;
        uint8_t minute;
        uint8_t hour;
        uint8_t day;
        uint8_t weekday;
        uint8_t month;
        uint16_t year;
        bool clockSet;
        bool battLow;
    };

    /// Use the first Clock module that's not already in use
    ClockModule();

    /// Use the clock module with the specified \a deviceID
    explicit ClockModule(uint16_t deviceID);
    
    /// Get the current date and time from the module
    Time getTime();

    /// Set the current date and time
    void setTime(const Time &t);
  
    /// Get the current ambient air temperature
    float getTemperatureC();
  
    float getTemperatureF();

 private:

};

#endif
