#include "Modulo.h"
#include "Wire.h"

// The knob modulo object
KnobModulo knob;

void setup() {
}

void loop() {
    Modulo.loop();

    // Get the angle of the knob
    float angle = knob.getAngle();

    // Convert the angle (between 0 and 260) to a hue (between 0 and 1)
    float hue = angle/360.0;

    // The saturation is 0 if the button is pressed and 1 otherwise.
    float saturation = 1-knob.getButton();

    // Set the color
    knob.setHSV(hue, saturation, 1.0);
}
