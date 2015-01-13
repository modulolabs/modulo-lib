#ifndef MODULO_H
#define MODULO_H

#include "ModIO.h"
#include "Broadcast.h"
#include "ModuloMiniDisplay.h"
#include "ModuloBase.h"

class ModDPad {
 public:
    ModDPad(uint8_t address);

    bool sync();

    bool getState(int button);
    uint8_t getStates();

    bool getPressed(int button);
    uint8_t getPresses();

    bool getReleased(int button);
    uint8_t getReleases();

 private:
    uint8_t _address, _state, _pressed, _released;
};

class ModClock : public ModBase {
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

    ModClock();
    explicit ModClock(uint16_t deviceID);

    Time getTime();
    void setTime(const Time &t);
  
    float getTemperature();
  
 private:

};

class ModKnob {

public:
    ModKnob(uint8_t address);

    bool setColor(float r, float g, float b);
    
    bool sync();
    bool getButtonState();
    bool getButtonPressed();
    bool getButtonReleased();
  
    int16_t getPosition();
  
 private:
    uint8_t _address;
    int16_t _position;
    bool _buttonState, _buttonPressed, _buttonReleased;
};

class ModThermocouple : public ModBase {
 public:
    explicit ModThermocouple(uint16_t deviceID);
    ModThermocouple();
  
    int16_t getTemperature();
    
    static const float InvalidTemperature;
};

class ModAC {
  float getCurrent();
  void setOutput(float level);
  
};

class ModMotor {

  // Set a single channel (0-3) to the specified amount, between 0 and 1.
  void setChannelOutput(uint8_t channel, float amount);
  
  // Set the motor output A to the specified amount, between -1 and 1.
  // Enables A0 and A1 if necessary.
  void setMotorA(float amount);
  
  // Set the motor output B to the specified amount, between -1 and 1.
  // Enables B0 and B1 if necessary.
  void setMotorB(float amount);
  
  // Set whether output drivers are enabled on outputs A0 and A1
  void setEnableA(bool enable);
  
  // Set whether output drivers are enabled on outputs B0 and B1
  void setEnableB(bool enable);
  
  // Set the PWM frequency for A0 and A1
  void setFrequencyA(uint16_t frequency);
  
  // Set the PWM frequency for B0 and B1
  void setFrequencyB(uint16_t frequency);
  
  // Set the stepper motor rotation speed
  void setStepperSpeed(float degPerSecond);

  // Step by the specified number of steps, either forward or back
  void beginStepper(int16_t steps);
  
  // Stop the stepper motor. This will leave the motor engaged
  void stopStepper();
  
  // Disable the stepper motor, allowing it to turn freely.
  void disableStepper();
  
  // Return the number of steps completed since the last call to step()
  int16_t getStepsCompleted();
};

#endif

