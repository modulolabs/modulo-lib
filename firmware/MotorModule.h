#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

#include "Module.h"

/// A driver for DC Motors, Stepper Motor, and other high current devices.
class MotorModule : public Module {
public:
    /// Use the first Motor  that's not already in use
    MotorModule();

    /// Use the Motor with the provided deviceID
    MotorModule(uint16_t deviceID);

    /// Set a single channel (0-3) to the specified amount, between 0 and 1.
    void setChannel(uint8_t channel, float amount);
    
    /// Set the motor output A to the specified amount, between -1 and 1.
    /// Enables A0 and A1 if necessary.
    void setMotorA(float amount);
    
    /// Set the motor output B to the specified amount, between -1 and 1.
    /// Enables B0 and B1 if necessary.
    void setMotorB(float amount);
    
    /// Set whether output drivers are enabled on outputs A0 and A1
    void setEnableA(bool enable);
    
    /// Set whether output drivers are enabled on outputs B0 and B1
    void setEnableB(bool enable);

  
};

#endif
