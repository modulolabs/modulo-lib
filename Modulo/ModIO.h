#ifndef MOD_IO_H
#define MOD_IO_H

#include <inttypes.h>

class ModIO {
public:
    enum AnalogReference {
        AREF_VCC,          // Bus power supply voltage
        AREF_1V1,  // 1.1V internal voltage reference
        AREF_2V2,  // 2.2V internal voltage reference
        AREF_4V,   // 4V internal voltage reference
        AREF_EXTERNAL      // External voltage reference on pin 7
    };

    ModIO(uint8_t address);

    // Disables the output on the specified pin and returns the pin's value
    bool getDigitalInput(uint8_t pin);
    
    // Reads the digital inputs from all 8 pins. Does not enable/disable outputs on any pins.
    uint8_t getDigitalInputs();
    
    // Disables the output on the specified pin and performs an analog read.
    float getAnalogInput(uint8_t pin, AnalogReference ref=AREF_VCC);  

    // Enables the output and sets the output value on the specified pin.
    void setDigitalOutput(uint8_t pin, bool value);

    // Set the digital outputs on all 8 pins. Does not enable or disable outputs on any pins.
    bool setDigitalOutputs(uint8_t values);

    // Enable the output and set the PWM duty cycle on the specified pin.
    // If the pin does not support PWM, this is equivalent to setDigitalOutput.
    void setPWMOutput(uint8_t pin, float value);

    // Sets whether a pullup is enabled on the specified pin.
    void setPullup(uint8_t pin, bool enable=true);  
  
    // Sets whether debounce is enabled on the specified pin.
    // If enabled, digital input changes must be stable for 5ms before they are recognized.
    void setDebounce(uint8_t pin, bool enable=true);

    // Set the frequency for PWM signals on the specified pin.
    void setPWMFrequency(uint8_t pin, uint16_t value);
    
private:
    uint8_t _address;
};

#endif

