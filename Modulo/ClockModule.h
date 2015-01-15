#ifndef CLOCK_MODULE_H
#define CLOCK_MODULE_H

#include "Module.h"

/// Keeps track of the current time and date, even without power. Also senses the ambient air temperature.
class ClockModule : public Module {
 public:
    struct Time {
    Time() : seconds(0), minutes(0), hours(0), days(0), weekdays(0), months(0), years(0), clockSet(false), battLow(false) {}
        
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t days;
        uint8_t weekdays;
        uint8_t months;
        uint16_t years;
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
    float getTemperature();
  
 private:

};

#endif
