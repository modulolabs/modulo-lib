#include <inttypes.h>

class TimeAndTemp {
public:
    struct Time {
        Time() : year(0), month(0), date(0), day(0), hour(0), minute(0), second(0) {}
        
        uint16_t year;
        uint8_t month;
        uint8_t date;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    };
  
    explicit TimeAndTemp(uint8_t address);
  
    Time LoadTime();
    void SaveTime(const Time& time);
    
    float GetTemperature();

private:
    uint8_t _address;
    
};
