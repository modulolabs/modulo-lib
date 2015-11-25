#include "Modulo.h"
#include "Wire.h"

DisplayModulo display(44056);


KnobModulo knob;
int knobColor = 0;
void onKnobButtonPressed(KnobModulo &knob) {
    knobColor = (knobColor+1) % 3;
}

void knobUpdate() {
    switch (knobColor) {
        case 0:
            knob.setColor(1,0,0);
            break;
        case 1:
            knob.setColor(0,1,0);
            break;
        case 2:
            knob.setColor(0,0,1);
            break;
    }
    display.print("Position: ");
    display.println(knob.getPosition());
}


JoystickModulo joystick;

void joystickUpdate() {
    display.println(joystick.getHPos());
    display.println(joystick.getVPos());
    display.println(joystick.getButton());
}

DisplayModulo testDisplay;

void displayUpdate() {

    if (testDisplay.getButton(0)) {
        testDisplay.fillScreen(255,0,0);
    }
    if (testDisplay.getButton(1)) {
        testDisplay.fillScreen(0,255,0);
    }
    if (testDisplay.getButton(2)) {
        testDisplay.fillScreen(0,0,255);
    }
    testDisplay.refresh();  
}

TemperatureProbeModulo tempProbe;


void onPositionReached(MotorDriverModulo &motor) {
    Serial.println("Position Reached");
    Serial.println(motor.getStepperPosition());
    delay(500);

    if (motor.getStepperPosition() <= 0) {
        motor.setStepperTarget(256*200);
    } else {
        motor.setStepperTarget(0);
    }

}

MotorDriverModulo motor;
long lastMotorUpdate = 0;
int motorPos = 0;
bool motorRunning = false;
void motorUpdate() {
/*
    motor.setEnable(true);

    if (millis() < lastMotorUpdate+10) {
        return;
    }

    motorPos++;

    switch (motorPos % 4) {
        case 0:
            motor.setMotorA(1);
            motor.setMotorB(0);
            break;
        case 1:
            motor.setMotorA(0);
            motor.setMotorB(1);
            break;
        case 2:
            motor.setMotorA(-1);
            motor.setMotorB(0);
            break;
        case 3:
            motor.setMotorA(0);
            motor.setMotorB(-1);
            break;
    }
*/
    if (!motorRunning) {
        motorRunning = true;
        motor.setStepperSpeed(100.0);
        motor.setMode(motor.ModeStepper);
        //motor.setCurrentLimit(.6);
        motor.setPWMFrequency(100);
        motor.setStepperTarget(256*200);
        delay(100);
    }
}

BlankSlateModulo blankSlate;

void setup() {
    Serial.begin(9600);
    display.getAddress();
    knob.setButtonPressCallback(onKnobButtonPressed);
    motor.setPositionReachedCallback(onPositionReached);


    Serial.println("Setup");

}

void loop() {
    Modulo.loop();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, (millis() % 1000) < 100);

    uint16_t deviceID = Modulo.getNextDeviceID(0);
    while (deviceID != 0xFFFF) {
        if (deviceID != display.getDeviceID()) {
            break;
        }
        deviceID = Modulo.getNextDeviceID(deviceID+1);
    }


    char deviceType[64] = {0};
    Modulo.getDeviceType(deviceID, deviceType, 64);


    display.clear();

    Modulo.setStatus(deviceID, ModuloStatusBlinking);

    display.setTextColor(255,255,255);

    if (strcmp(deviceType,"co.modulo.knob") == 0) {
        display.println("Knob");
        display.println(deviceID);

        knob.setDeviceID(deviceID);
        knobUpdate();
    }

    else if (strcmp(deviceType,"co.modulo.joystick") == 0) {
        display.println("Joystick");
        display.println(deviceID);
        if (joystick.getDeviceID() == 0xFFFF) {
            joystick.setDeviceID(deviceID);
        }
        joystickUpdate();
    }

    else if (strcmp(deviceType,"co.modulo.colordisplay") == 0) {
        display.println("Display");
        display.println(deviceID);

        displayUpdate();
    }

    else if (strcmp(deviceType, "co.modulo.tempprobe") == 0) {
        display.println("Temperature");

        tempProbe.setDeviceID(deviceID);
        display.println(tempProbe.getTemperatureF());
    }
    else if (strcmp(deviceType, "co.modulo.motor") == 0) {
        display.println("Motor");
        if (display.getButton(0)) {
            motor.setCurrentLimit(.1);
        } else if (display.getButton(2)) {
            motor.setCurrentLimit(1);
        }
        motorUpdate();
    }
    else if (strcmp(deviceType, "co.modulo.ir") == 0) {
        display.println("IR");
    }
    else if (strcmp(deviceType, "co.modulo.io") == 0) {
        display.println("Blank Slate");
        blankSlate.setDirections(0xFF);
        int pin = (millis()/250) % 8;

        blankSlate.setDigitalOutputs(1 << pin);
    } else {
        display.println("Device Type:");
        display.println(deviceType);
    }

    if (deviceID == 0xFFFF) {
        display.println("No Modulo!");

        for (int i=0; i < 6; i++) {
            pinMode(i, INPUT_PULLUP);
            display.print(digitalRead(i));
        }
        display.println();

        display.println("Display:");
        display.println(display.getDeviceID());
    }

    display.refresh();


    Modulo.exitBootloader();
    delay(100);
}
