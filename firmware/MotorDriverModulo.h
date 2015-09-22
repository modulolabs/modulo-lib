#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

#include "BaseModulo.h"


/// A driver for DC Motors, Stepper Motor, and other high current devices.
class MotorDriverModulo : public BaseModulo {
public:
    /// Use the first Motor  that's not already in use
    MotorDriverModulo();

    /// Use the Motor with the provided deviceID
    MotorDriverModulo(uint16_t deviceID);

    /// Set a single channel (0-3) to the specified amount, between 0 and 1.
    void setChannel(uint8_t channel, float amount);
    
    /// Set the motor output A to the specified amount, between -1 and 1.
    /// Enables A0 and A1 if necessary.
    void setMotorA(float amount);
    
    /// Set the motor output B to the specified amount, between -1 and 1.
    /// Enables B0 and B1 if necessary.
    void setMotorB(float amount);
    
    /// Set whether output drivers are enabled
    void setEnable(bool enable);

};

#endif
