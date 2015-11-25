#include "MotorDriverModulo.h"
#include "Modulo.h"

static float _constrain(float v, float min, float max) {
    if (v < min) {
        return min;
    }
    if (v > max) {
        return max;
    }
    return v;
}


static const uint8_t _FunctionSetValue = 0;
static const uint8_t _FunctionSetEnabled = 1;
static const uint8_t _FunctionSetFrequency = 2;
static const uint8_t _FunctionSetCurrentLimit = 3;
static const uint8_t _FunctionSetStepperSpeed = 4;
static const uint8_t _FunctionGetStepperPosition = 5;
static const uint8_t _FunctionSetStepperTarget = 6;
static const uint8_t _FunctionAddStepperOffset = 7;

static const uint8_t _EventPositionReached = 0;
static const uint8_t _EventFaultChanged = 1;

MotorDriverModulo::MotorDriverModulo() : BaseModulo("co.modulo.motor"),
    _fault(false), _stepperOffset(0), _usPerStep(5000), _microsteps(256), _minMicrostepDuration(1000),
    _positionReachedCallback(NULL), _faultChangedCallback(NULL)
{
}

MotorDriverModulo::MotorDriverModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.motor", deviceID),
    _fault(false), _stepperOffset(0), _usPerStep(5000), _microsteps(256), _minMicrostepDuration(1000),
    _positionReachedCallback(NULL), _faultChangedCallback(NULL)
{
}

void MotorDriverModulo::setChannel(uint8_t channel, float amount) {
    uint16_t intValue = _constrain(amount, 0, 1)*0xFFFF;
    uint8_t data[] = {channel, static_cast<uint8_t>(intValue), static_cast<uint8_t>(intValue >> 8)};
    _transfer(_FunctionSetValue, data, 3, 0, 0);
}

void MotorDriverModulo::setMotorA(float value) {
    if (value > 0) {
        setChannel(0, 1);
        setChannel(1, 1-value);
    } else {
        setChannel(0, 1+value);
        setChannel(1, 1);
    }
}

void MotorDriverModulo::setMotorB(float value) {
    if (value > 0) {
        setChannel(2, 1);
        setChannel(3, 1-value);
    } else {
        setChannel(2, 1+value);
        setChannel(3, 1);
    }
}

void MotorDriverModulo::setMode(Mode mode) {
    uint8_t dataToSend[] = {static_cast<uint8_t>(mode)};
    _transfer(_FunctionSetEnabled, dataToSend, 1, 0, 0);
}

void MotorDriverModulo::setCurrentLimit(float limit) {
    limit = _constrain(limit, 0.0, 1.0);
    uint8_t dataToSend[] = {limit*63};
    _transfer(_FunctionSetCurrentLimit, dataToSend, 1, 0, 0);
}

void MotorDriverModulo::setStepperSpeed(float stepsPerSecond) {
    setStepperRate(1000000/stepsPerSecond);
}

void MotorDriverModulo::setStepperRate(uint32_t usPerStep) {
    _usPerStep = usPerStep;

    _updateStepperSpeed();
}

void MotorDriverModulo::_updateStepperSpeed() {
    int microsteps = _microsteps;

    // Can do at most 256 microsteps/step
    if (microsteps > 256) {
        microsteps = 256;
    }

    // If necessary, decrease the number of microsteps to keep the microstep
    // duration above minMicrostepDuration.
    while (microsteps > 1 and _usPerStep/microsteps < _minMicrostepDuration) {
/*
        Serial.print("Decreasing microsteps from ");
        Serial.print(microsteps);
        Serial.print(" to ");
        Serial.println(microsteps/2);
*/
        microsteps /= 2;
    }

    // Determine the microstep resolution.
    // It's the base 2 logarithm of microsteps.
    uint8_t resolution = 0;
    for (int i = microsteps/2 ; i > 0 and resolution <= 8; i /= 2) {
        resolution++;
    }

    // Determine the number of 8us ticks per microstep
    uint32_t ticksPerMicrostep = _usPerStep/(8*microsteps);
    
    // Clamp ticksPerMicrostep to 16 bits
    if (ticksPerMicrostep > 65535) {
        ticksPerMicrostep = 65535;
    }
    
/*
    Serial.print(ticksPerMicrostep*8);
    Serial.print(" ");
    Serial.println(microsteps);
*/

    uint8_t sendData[] = {
        static_cast<uint8_t>(ticksPerMicrostep),
        static_cast<uint8_t>(ticksPerMicrostep >> 8),
        static_cast<uint8_t>(resolution)
    };

    _transfer(_FunctionSetStepperSpeed, sendData, 3, 0, 0);

}

int32_t MotorDriverModulo::getStepperPosition() {
    uint8_t receivedData[4];
    uint32_t pos = 0;
    if (_transfer(_FunctionGetStepperPosition, 0, 0, receivedData, 4)) {
        for (int i=3; i >= 0; i--) {
            pos <<= 8;
            pos |= receivedData[i];
        }
    }
    return pos;

}

void MotorDriverModulo::setStepperTarget(int32_t targetPosition) {
    uint8_t sendData[4] = {
        static_cast<uint8_t>(targetPosition),
        static_cast<uint8_t>(targetPosition >> 8),
        static_cast<uint8_t>(targetPosition >> 16),
        static_cast<uint8_t>(targetPosition >> 24)};

    _transfer(_FunctionSetStepperTarget, sendData, 4, 0, 0);
}

void MotorDriverModulo::setPWMFrequency(uint16_t frequency) {
    uint8_t sendData[2] = {
        static_cast<uint8_t>(frequency),
        static_cast<uint8_t>(frequency >> 8)};

    _transfer(_FunctionSetFrequency, sendData, 2, 0, 0);
}

void MotorDriverModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {
    if (eventCode == _EventPositionReached) {
        if (_positionReachedCallback) {
            _positionReachedCallback(*this);
        }
    }

    if (eventCode == _EventFaultChanged) {
        if (eventData & 1) {
            _fault = true;
        }
        if (eventData & 2) {
            _fault = false;
        }

        if (_faultChangedCallback) {
            _faultChangedCallback(*this);
        }
    }
}

void MotorDriverModulo::setPositionReachedCallback(EventCallback *handler) {
    _positionReachedCallback = handler;
}

/// Set the function that should be called when a fault occurs or is cleared
void MotorDriverModulo::setFaultChangedCallback(EventCallback *handler) {
    _faultChangedCallback = handler;
}

