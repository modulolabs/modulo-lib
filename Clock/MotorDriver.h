#include <inttypes.h>

class MotorDriver {
public:
    explicit MotorDriver(uint16_t address);
  
    void SetEnableA(bool enable);
    void SetEnableB(bool enable);
    
    void SetFrequencyA(uint16_t frequency);
    void SetFrequencyB(uint16_t frequency);
    
    void SetOutputA1(uint16_t value);
    void SetOutputA2(uint16_t value);
    void SetOutputB1(uint16_t value);
    void SetOutputB2(uint16_t value);
    
    void SetStatus(uint8_t status);
    
private:
    uint8_t _address;    
};
