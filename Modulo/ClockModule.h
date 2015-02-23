#ifndef CLOCK_MODULE_H
#define CLOCK_MODULE_H

#include "Module.h"

class Print;


/// Keeps track of the current time and date, even without power. Also senses the ambient air temperature.
class ClockModule : public Module {
 public:
    /// Represents a time (and date)
    struct Time {
    Time() : second(0), minute(0), hour(0), day(0), weekday(0), month(0), year(0), clockSet(false), battLow(false) {}
        
        void printDay(Print &p);
        void printDate(Print &p);
        void printTime(Print &p);
        
        /// Seconds
        uint8_t second;

        /// Minute [0, 59]
        uint8_t minute;

        /// Hour [0, 23]
        uint8_t hour;

        /// Day [0, 31]
        uint8_t day;

        /// Weekday [0, 6]
        uint8_t weekday;

        /// Month [1, 12]
        uint8_t month;

        /// Year
        uint16_t year;

        /// Clock has been set. If this is false, all other values are invalid.
        bool clockSet;

        /// Battery low.
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
  
    /// Get the current ambient air temperature in celsius
    float getTemperatureC();
  
    /// Get the current ambient air temperature in fahrenheit
    float getTemperatureF();

 private:

};

#endif
