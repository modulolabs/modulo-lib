#include "Modulo.h"
#include "Wire.h"
#include <string.h>

KnobModulo knob;
JoystickModulo joystick;
DisplayModulo display;

int knobPosition = 0;
bool knobButton = false;
int volume = 1;
bool updateVolumeDisplay = true;

void drawVolumeDisplay() {
    display.setLineColor(0, 0, 0);
    display.setFillColor(0, 0, 0);
    display.drawRect(0,0,35,50);

    for (int i=0; i < volume; i++) {
        display.setFillColor(25*i, 255-25*i, 255*knob.getButton());
        display.drawRect(5, 40-4*i, 25, 4);
    }

    knob.setColor(volume/10.0, 1-volume/10.0, knob.getButton());
}

void onKnobChanged(KnobModulo &knob) {
    volume += knob.getPosition()-knobPosition;
    knobPosition = knob.getPosition();

    volume = constrain(volume, 1, 10);

    knobButton = knob.getButton();
}

DisplayModulo::Color activeButtonColor(255,0,0);
DisplayModulo::Color inactiveButtonColor(0,0,255);
int currentButton = 1;


void drawDisplayDisplay() {

    int buttons = display.getButtons();
    if (buttons == 1) {
        currentButton = 0;
    } else if (buttons == 2) {
        currentButton = 1;
    } else if (buttons == 4) {
        currentButton = 2;
    }



    display.setTextColor(currentButton == 0 ? activeButtonColor : inactiveButtonColor);
    display.setCursor(0, 56);
    display.print("A");
    
    display.setTextColor(currentButton == 1 ? activeButtonColor : inactiveButtonColor);
    display.setCursor(44, 56);
    display.print("B");

    display.setTextColor(currentButton == 2 ? activeButtonColor : inactiveButtonColor);
    display.setCursor(90, 56);
    display.print("C");
}

void drawJoystickDisplay() {
    display.setLineColor(0,0,0);
     
    display.setFillColor(40, 40, 40);
    display.drawRect(50, 5, 40, 40);

    if (joystick.getButton()) {
//        display.setFillColor(120, 0, 90);
        display.setFillColor(0,0,255);
    } else {
        display.setFillColor(128,128,128);
    }

    display.drawRect(
        60 - joystick.getHPos()*10,
        15 + joystick.getVPos()*10,
        20, 20);
}

bool updateJoystickDisplay = true;

void onJoystickChanged(JoystickModulo &j) {
    updateJoystickDisplay = true;
}

void setup() {
    knob.setPositionChangeCallback(onKnobChanged);
    knob.setButtonPressCallback(onKnobChanged);
    knob.setButtonReleaseCallback(onKnobChanged);

    joystick.setButtonPressCallback(onJoystickChanged);
    joystick.setButtonReleaseCallback(onJoystickChanged);
    joystick.setPositionChangeCallback(onJoystickChanged);

    pinMode(LED_BUILTIN, OUTPUT);
}

long lastDrawTime = 0;
void loop() {
    long newDrawTime = millis();

    digitalWrite(LED_BUILTIN, (millis()/250) % 2);
    Modulo.loop();

    display.clear();


    drawDisplayDisplay();

    if (updateVolumeDisplay) {
        drawVolumeDisplay();
    }

    if (updateJoystickDisplay) {
        drawJoystickDisplay();
    }

    display.setCursor(0,0);
    //display.drawRect(0,0,20,10);
    display.println(1000.0/(newDrawTime-lastDrawTime));

    lastDrawTime = newDrawTime;

    display.refresh();
}