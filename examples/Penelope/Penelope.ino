#include "Modulo.h"
#include "Wire.h"
#include "ModuloServo.h"

DisplayModulo display;
KnobModulo timeKnob;
KnobModulo armKnob;

JoystickModulo joystick;
MotorDriverModulo wheelMotors;
MotorDriverModulo solenoidMotors;
TemperatureProbeModulo tempProbe;

Servo servo;

int duration = 10000;
int timeKnobPos = 0;
int armKnobPos = 0;
int armDownValue = 60;
int armUpValue = 90;
float armPos = 1;

enum State {
    Idle,
    BeginBrewing,
    Brewing,
    EndBrewing
};

State state = Idle;

void onTimeKnobPressed(KnobModulo &knob) {
    switch (state) {
    case Idle :
        state = BeginBrewing;
        break;
    case BeginBrewing:
        state = EndBrewing;
        break;
    case Brewing:
        state = EndBrewing;
        break;
    case EndBrewing:
        state = Brewing;
        break;
    }
}

void onTimeKnobPositionChanged(KnobModulo &knob) {
    if (state == Idle) {
        duration += 1000*(timeKnob.getPosition()-timeKnobPos);
        duration = max(0, duration);
        timeKnobPos = timeKnob.getPosition();
    }
}

void onArmKnobPositionChanged(KnobModulo &knob) {
    if (state == Idle) {
        armPos += .05*(knob.getPosition()-armKnobPos);
        armKnobPos = knob.getPosition();

        // Ensure that armPos stays between 0 and 1
        armPos = constrain(armPos, 0, 1);
    }    
}

void onJoystickPositionChanged(JoystickModulo &j) {
    float hPos = j.getHPos();
    float vPos = j.getVPos();
    
    float maxSpeed = .45;

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

    wheelMotors.setEnable(true);
    wheelMotors.setMotorA(motorA);
    wheelMotors.setMotorB(motorB);
}

void ringBell() {
    solenoidMotors.setEnable(true);
    solenoidMotors.setMotorA(1);
    delay(10);
    solenoidMotors.setMotorA(0);
}

void onJoystickButtonPressed(JoystickModulo &j) {
    ringBell();
}

void setup() {
    servo.attach(1);

    timeKnob.setButtonPressCallback(onTimeKnobPressed);
    armKnob.setButtonPressCallback(onTimeKnobPressed);

    timeKnob.setPositionChangeCallback(onTimeKnobPositionChanged);
    joystick.setPositionChangeCallback(onJoystickPositionChanged);
    armKnob.setPositionChangeCallback(onArmKnobPositionChanged);
    joystick.setButtonPressCallback(onJoystickButtonPressed);
}

long newLoopTime = 0;
long brewingStartTime = 0;

int previousTimeDisplay = -1;
float previousArmPos = -1;
int previousTemperature = -1;

bool ringBellWhenDone = false;

void loop() {
    long lastLoopTime = newLoopTime;
    newLoopTime = millis();
    float dt = (newLoopTime-lastLoopTime)/1000.0;

    Modulo.loop();

    long timeDisplay = duration;

    switch (state) {
        case Idle:
            break;
        case BeginBrewing:
            // Raise the arm in 1 seconds
            armPos += dt/1;
            if (armPos >= 1) {
                armPos = 1;
                state = Brewing;
                brewingStartTime = millis();
            }
            break;
        case Brewing:
            ringBellWhenDone = false;
            armPos = .5 + .5*cos((millis()-brewingStartTime)/500.0);
            if (millis()-brewingStartTime > duration and armPos > .9) {
                ringBellWhenDone = true;
                state = EndBrewing;
            }

            timeDisplay = duration + brewingStartTime - millis();
            timeDisplay = max(0, timeDisplay);
            break;
        case EndBrewing:
            // Raise the arm in 1 seconds
            armPos += dt/1;
            if (armPos >= 1) {
                if (ringBellWhenDone) {
                    ringBell();
                }
                armPos = 1;
                state = Idle;
            }
            break;
    }

    servo.write(armPos*armUpValue + (1-armPos)*armDownValue);


    armKnob.setColor(0,0,1);
    timeKnob.setColor(1,0,1);

    int newTemperature = tempProbe.getTemperatureF();
    if (timeDisplay != previousTimeDisplay or
        armPos != previousArmPos or
        newTemperature != previousTemperature) {
        previousTimeDisplay = timeDisplay;
        previousArmPos = armPos;
        previousTemperature = newTemperature;

        display.clear();

        display.setTextColor(255,0,255);
        display.setTextSize(1);
        display.println("Duration: ");
        display.setTextSize(2);
        display.println(timeDisplay/1000);
        display.println();

        display.setTextColor(255,0,0);
        display.setTextSize(1);
        display.setCursor(60,0);
        display.print("Temp");
        display.setTextSize(2);
        display.setCursor(60,8);
        display.println(previousTemperature);

        display.setTextColor(0,0,255);
        display.setTextSize(1);
        display.println("\nPosition: ");
        display.setTextSize(2);

        display.println(armPos);

        display.refresh();
    }


}

