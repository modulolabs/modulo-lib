#include "Modulo.h"
#include "Wire.h"

// An object for each modulo
MotorDriverModulo motorDriver;
JoystickModulo joystick;

void setup() {

}

void loop() {
    Modulo.loop();

    // Get the horizontal and vertical position if the joystick
    float hpos = joystick.getHPos();
    float vpos = joystick.getVPos();

    // Set the speed of motor A. 
    motorDriver.setMotorA(vpos + hpos);
    motorDriver.setMotorB(vpos - hpos);
}
