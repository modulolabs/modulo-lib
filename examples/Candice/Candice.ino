#include "Modulo.h"
#include "Wire.h"
#include "ModuloServo.h"

DisplayModulo display;
KnobModulo knob;
IRRemoteModulo ir;
Servo servo;

bool hasPermission = false;
bool buttonPressed = false;
bool knobPressed = false;
bool dispensing = false;
int dispensingStart = 0;

int servoClosedPos = 170;
int servoOpenPos = 15;
int openDuration = 1500;
int closeDuration = 1500;


void setup() {
    servo.attach(1);
    servo.write(servoClosedPos);
    delay(500);
    servo.detach();
    
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

    Modulo.loop();
    digitalWrite(LED_BUILTIN, (millis()/500) % 2);

    bool hasCup = !digitalRead(0);

    if (display.getButton(2) != buttonPressed) {
        buttonPressed = !buttonPressed;
        if (buttonPressed) {
            hasPermission = !hasPermission;
        }
    }

    
    int8_t irProtocol = 0;
    uint32_t irCode = 0;
    if (ir.receive(&irProtocol, &irCode)) {
        if (irCode == 14622959) {
            hasPermission = !hasPermission;
        }
    }

    if (knob.getButton() != knobPressed) {
        knobPressed = !knobPressed;
        if (knobPressed and hasCup and hasPermission) {
            dispensing = true;
    
            dispensingStart = millis();
        }
    }


    display.clear();
    display.setCursor(0,0);
    if (dispensing) {
        display.fillScreen(0,0,255);
        display.setTextSize(2);
        display.setTextColor(255,255,255);
        display.setCursor(10,10);
        display.println("Candy!");

        if (millis() < dispensingStart+openDuration) {
            servo.attach(1);
            servo.write(servoOpenPos);
        } else if (millis() < dispensingStart+openDuration+closeDuration) {
            servo.write(servoClosedPos);
        } else {
            servo.detach();
            dispensing = false;
        }

        knob.setHSV(millis()/1000.0, 1, 1);

    } else if (!hasPermission) {
        display.fillScreen(255,0,0);
        display.setTextSize(2);
        display.setTextColor(255,255,255);
        display.setCursor(10,10);
        display.println("Ask");

        display.setCursor(10,30);
        display.println("nicely");
        knob.setColor(1,0,0);

    } else if (!hasCup) {
        display.fillScreen(255,255,0);
        display.setTextSize(2);
        display.setTextColor(0,0,0);
        display.setCursor(10,10);
        display.println("Place");

        display.setCursor(10,30);
        display.println("Cup");
    
        knob.setColor(1,1,0);
    } else {
        display.fillScreen(0,200,0);
        display.setTextSize(2);
        display.setTextColor(255,255,255);
        display.setCursor(10,10);
        display.println("Ready!");

        display.setTextSize(1);
        display.setCursor(5, 30);
        display.println("Press the Knob");
        
        knob.setColor(0,1,0);
    }
    display.refresh();
}
