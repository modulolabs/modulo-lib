#include "Modulo.h"
#include "Wire.h"

// Create an object that represents the display
DisplayModulo display;

// Declare a function that will be called when a button is pressed
void onButtonPress(DisplayModulo &d, int button);

// Declare a function that will be called when a button is release
void onButtonRelease(DisplayModulo &d, int button);


// The setup function is run once, when the program starts
void setup() {
    // Call Modulo.setup() before using Modulo devices in the setup function,
    // as we're doing by calling onButtonRelease() below.
    Modulo.setup();

    // Register our callback functions
    display.setButtonPressCallback(onButtonPress);
    display.setButtonReleaseCallback(onButtonRelease);

    // Call onButtonRelease to draw the initial text
    onButtonRelease(display, 0);
}

// The loop function is run constantly
void loop() {
    // Always call Modulo.loop() at the top of the loop function. It
    // communicates with the modulos and executes callbacks.
    //
    // Since we're using events, that's all that needs to happen in loop()!
    Modulo.loop();
}

// This callback function will run when a button is pressed.
void onButtonPress(DisplayModulo &d, int button) {
    // Clear the display
    display.clear();

    // Draw different text, in a different color, based on which button is pressed.
    switch (button) {
        case 0:
            display.setTextColor(1,0,0);
            display.println("Button 0");
            break;
        case 1:
            display.setTextColor(0,1,0);
            display.println("Button 1");
            break;
        case 2:
            display.setTextColor(0,0,1);
            display.println("Button 2");
            break;
    }

    // Nothing will actually show up on the display until you call refresh.
    display.refresh();
}

void onButtonRelease(DisplayModulo &d, int button) {
    // Clear the display
    display.clear();

    // Write "none pressed" in white.
    display.setTextColor(1,1,1);
    display.println("None pressed");        

    // Update the display with the new text
    display.refresh();
}

