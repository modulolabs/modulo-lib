#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

#include "Module.h"

/// A driver for DC Motors, Stepper Motor, and other high current devices.
class MotorModule : public Module {

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
