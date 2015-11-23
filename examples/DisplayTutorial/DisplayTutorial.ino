#include "Modulo.h"
#include "Wire.h"

// Create an object that represents the display
DisplayModulo display;

// The setup function is run once, when the program starts
void setup() {
}

// The loop function is run constantly
void loop() {
    // Always call Modulo.loop() at the top of the loop function. It
    // communicates with the modulos and executes callbacks if any events
    // have occured.
    Modulo.loop();

    // Clear the display
    display.clear();

    // Draw different text, in a different color, based on which button is pressed.
    if (display.getButton(0)) {
        display.setTextColor(255,0,0);
        display.println("Button 0");
    } else if (display.getButton(1)) {
        display.setTextColor(0,255,0);
        display.println("Button 1");
    } else if (display.getButton(2)) {
        display.setTextColor(0,0,255);
        display.println("Button 2");
    } else {
        display.setTextColor(255,255,255);
        display.println("None pressed");        
    }

    // Nothing will actually show up on the display until you call refresh.
    display.refresh();
}
