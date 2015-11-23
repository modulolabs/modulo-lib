// Include the Modulo library and the Wire library, which it depends on.
#include "Modulo.h"
#include "Wire.h"

// Create an object that represents each modulo
MotorDriverModulo motorDriver;
JoystickModulo joystick;

// Declare a function that will be called when the joystick moves
void onJoystickChanged(JoystickModulo &k);


// The setup function is run once, when the program starts
void setup() {

}

// The loop function is run constantly
void loop() {
    // Always call Modulo.loop() at the top of the loop function. It
    // communicates with the modulos and executes callbacks.
    //
    // Since we're using events, that's all that needs to happen in loop()!
    Modulo.loop();
}

void onJoystickChanged(JoystickModulo &j) {
    // Get the horizontal and vertical position if the joystick
    float hpos = j.getHPos();
    float vpos = j.getVPos();

    // Set the speed of motor A. 
    motorDriver.setMotorA(vpos + hpos);
    motorDriver.setMotorB(vpos - hpos);
}
