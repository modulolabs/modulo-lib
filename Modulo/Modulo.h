#ifndef MODULO_H
#define MODULO_H

#include "ModIO.h"

bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries=3);
    
    
    
class ModDPad {
  bool getState(int button);
  uint8_t getStates();

  bool getPressed(int pin);
  uint8_t getPresses();

  bool getReleased(int pin);
  uint8_t getReleases();


};

class ModTime {
public:
  struct Time {
        Time() : seconds(0), minutes(0), hours(0), days(0), weekdays(0), months(0), years(0), clockSet(false), battLow(false) {}
        
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t days;
        uint8_t weekdays;
        uint8_t months;
        uint8_t years;
        bool clockSet;
        bool battLow;
  };

  ModTime(uint8_t address);

  Time getTime();
  void setTime(const Time &t);
  
  float getTemperature();
  
private:
  uint8_t _address;
};

class ModKnob {

  void setColor(float r, float g, float b);
  
  bool getButtonState();
  bool getButtonPressed();
  bool getButtonReleased();
  
  uint8_t getPosition();
  int16_t getRotations();
  
};

class ModThermocouple {
public:
  ModThermocouple(uint8_t address);
    
  
  float getTemperatureA();
  float getTemperatureB();
private:
  uint8_t _address;
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

