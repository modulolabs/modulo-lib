#include "Modulo.h"
#include "Wire.h"

JoystickModulo joystick;
MotorDriverModulo wheelMotors;


void onJoystickPositionChanged(JoystickModulo &j) {
    float hPos = j.getHPos();
    float vPos = j.getVPos();
    
    float maxSpeed = .7;
    wheelMotors.setEnable(true);
    float motorA = vPos + hPos;
    float motorB = vPos - hPos;
    if (motorA > .1) {
        motorA = maxSpeed;
    } else if (motorA < -.1) {
        motorA = -maxSpeed;
    }

    if (motorB > .1) {
        motorB = maxSpeed;
    } else if (motorB < -.1) {
        motorB = -maxSpeed;
    }

    wheelMotors.setMotorA(motorA);
    wheelMotors.setMotorB(motorB);

}

void setup() {
    joystick.setPositionChangeCallback(onJoystickPositionChanged);
}

void loop() {
    wheelMotors.setEnable(true);
    Modulo.loop();
}
