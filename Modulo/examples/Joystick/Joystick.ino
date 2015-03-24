#include "Wire.h"
#include "Modulo.h"
#include <avr/io.h>

MiniDisplayModule display;
JoystickModule joystick;

void setup() {
    Serial.begin(9600);
    ModuloSetup();
}

void loop() {
    ModuloLoop();
    display.clear();
    display.println(joystick.getButton());
    display.println(joystick.getHPos());
    display.println(joystick.getVPos());
       display.drawRect(joystick.getHPos()*30 + display.WIDTH/2,
        display.HEIGHT/2+joystick.getVPos()*30,
        2,2, 1);

    display.refresh();


}