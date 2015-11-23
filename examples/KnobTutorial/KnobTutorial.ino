// Include the Modulo library and the Wire library, which it depends on.
#include "Modulo.h"
#include "Wire.h"

// Create an object that represents the knob
KnobModulo knob;

// The setup function is run once, when the program starts
void setup() {
}

// The loop function is run constantly
void loop() {
    // Always call Modulo.loop() at the top of the loop function. It
    // communicates with the modulos and executes callbacks if any events
    // have occured.
    Modulo.loop();

    // Get the angle of the knob. The angle is between 0 and 360
    float angle = knob.getAngle();

    // Convert the angle (between 0 and 360) to a hue (between 0 and 1)
    float hue = angle/360.0;

    // The saturation is 0 if the button is pressed and 1 otherwise.
    float saturation = 1-knob.getButton();

    // Set the knob's color
    knob.setHSV(hue, saturation, 1.0);
}
