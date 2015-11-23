// Include the Modulo library and the Wire library, which it depends on.
#include "Modulo.h"
#include "Wire.h"

// Declare a function that will be called when the knob is turned or pressed
void onKnobChanged(KnobModulo &k);

// Create an object that represents the knob
KnobModulo knob;

// The setup function is run once, when the program starts
void setup() {
    // Attach the callback function to the knob. We're interested in three
    // types of events: button pressed, button release, and position changed.
    knob.setButtonPressCallback(onKnobChanged);
    knob.setButtonReleaseCallback(onKnobChanged);
    knob.setPositionChangeCallback(onKnobChanged);

    // Run the callback once to set the initial color
    onKnobChanged(knob);
}

// The loop function is run constantly
void loop() {
    // Always call Modulo.loop() at the top of the loop function. It
    // communicates with the modulos and executes callbacks.
    //
    // Since we're using events, that's all that needs to happen in loop()!
    Modulo.loop();
}

// This is our callback function. It will run when the knob is presed or turned.
void onKnobChanged(KnobModulo &k) {
    // Get the angle of the knob. The angle is between 0 and 360
    float angle = k.getAngle();

    // Convert the angle (between 0 and 360) to a hue (between 0 and 1)
    float hue = angle/360.0;

    // The saturation is 0 if the button is pressed and 1 otherwise.
    float saturation = 1-k.getButton();

    // Set the knob's color
    k.setHSV(hue, saturation, 1.0);
}

