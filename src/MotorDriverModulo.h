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
    
    /// Set the motor output A to the specified amount, between -1 and 1.
    /// Changes the mode to ModeDC if it's not already.
    void setMotorA(float amount);
    
    /// Set the motor output B to the specified amount, between -1 and 1.
    /// Changes the mode to ModeDC if it's not already.
    void setMotorB(float amount);
    
    // Sets an individual output, making it possible to drive up to four loads
    // with a single motor driver. Sets the mode to DC if it's not already.
    //
    // The motor driver cannot drive more than one output in a pair to the power
    // supply voltage at the same time. Because of this you should connect the
    // load between the motor driver output and the power supply, not between
    // the motor driver output and ground.
    //
    // value must be between 0 and 1, and is the percentage of time that the
    // output will NOT be driven to ground. (that is, it's either driven to the
    // power supply voltage or it's disconnected based on the state of the other
    // output in the pair)
    //
    // channel number must be between 0 and 3, corresponding to terminals A1,
    // A2, B1, or B2 respectively.
    void setOutput(uint8_t channel, float amount);

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

    /// Return whether a fault condition (such as a short between motor terminals,
    /// over current shutdown, or over temperature shutdown) is currently present.
    bool hasFault();

    /// A callback function
    typedef void (EventCallback)(MotorDriverModulo &module);

    /// Set the function that should be called when the stepper target position
    /// is reached.
    void setPositionReachedCallback(EventCallback *handler);

    /// Set the function that should be called when a fault occurs or is cleared
    void setFaultChangedCallback(EventCallback *callback);

    /// Set the an input to te DRV8848 driver IC to a particular PWM value,
    /// between 0 and 1. Changes the mode to ModeDC if it's not already. 
    ///
    /// This is a low level function and may not do what you expect. If you are
    /// trying to drive single-ended loads, use setOutput instead.
    void setChannel(uint8_t channel, float amount);

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
