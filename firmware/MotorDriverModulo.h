#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

#include "BaseModulo.h"


/// A driver for DC Motors, Stepper Motor, and other high current devices.
class MotorDriverModulo : public BaseModulo {
public:
    enum Mode {
        ModeDisabled, // Output drivers are disconnected
        ModeDC,       // Drives two DC motors or 4 independent DC channels.
        ModeStepper   // Drives a stepper motor
    };

    /// Use the first MotorDriverModulo that's not already in use.
    MotorDriverModulo();

    /// Use the Motor with the provided deviceID.
    MotorDriverModulo(uint16_t deviceID);

    /// Set a single channel (0-3) to the specified amount, between 0 and 1.
    /// Changes the mode to ModeDC if it's not already.
    void setChannel(uint8_t channel, float amount);
    
    /// Set the motor output A to the specified amount, between -1 and 1.
    /// Changes the mode to ModeDC if it's not already.
    void setMotorA(float amount);
    
    /// Set the motor output B to the specified amount, between -1 and 1.
    /// Changes the mode to ModeDC if it's not already.
    void setMotorB(float amount);
    
    /// Set the driver mode.
    void setMode(Mode mode);

    /// Set the driver curren limit (between 0 and 1).
    void setCurrentLimit(float limit);

    /// Set the motor driver PWM frequency
    void setPWMFrequency(uint16_t frequency);

    /// Set the stepper speed in whole steps per second.
    void setStepperSpeed(float stepsPerSecond);

    /// Set the number of microseconds to take between each whole step.
    void setStepperRate(uint32_t usPerStep);

    /// Set the number of microsteps to take between each whole step.
    /// It can be 1, 2, 4, 8, 16, 32, 64, 128, or 256.
    /// 
    /// If the duration of a microstep (in microseconds) would be less than
    /// minMicrostepDuration, then the number of microsteps is decreased
    /// automatically. This helps to avoid skipping steps when the rate is
    /// higher than the motor or driver can handle.
    void setStepperResolution(int microsteps,
        uint16_t minMicrostepDuration=1000);


    /// Set the stepper target position. The target position is in 1/256 of
    /// a whole step. (So setting the target to 256 will take as many steps/
    /// microsteps as are necessary to move to the position that is 1 whole
    /// step from the starting position.)
    void setStepperTarget(int32_t targetPosition);

    /// Return the current position of the stepper motor in 1/256 increments
    /// of wholes steps.
    int32_t getStepperPosition();

    /// Return whether the stepper driver is currently stepping and has not
    /// yet reached its target position.
    bool isStepping();

    bool hasFault();

    /// A callback function
    typedef void (EventCallback)(MotorDriverModulo &module);

    /// Set the function that should be called when the stepper target position
    /// is reached.
    void setPositionReachedCallback(EventCallback *handler);

    /// Set the function that should be called when a fault occurs or is cleared
    void setFaultChangedCallback(EventCallback *callback);

private:
    void _updateStepperSpeed();
    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);
    

    bool _fault;

    uint32_t _stepperOffset;

    uint32_t _usPerStep;
    int _microsteps;
    uint16_t _minMicrostepDuration;
    EventCallback *_positionReachedCallback;
    EventCallback *_faultChangedCallback;


};

#endif
