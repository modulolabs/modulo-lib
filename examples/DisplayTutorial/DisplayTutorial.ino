#include "Modulo.h"
#include "Wire.h"

// The display modulo object
DisplayModulo display;
DisplayModulo::Color textColor(255,255,255);

void setup() {
}

void loop() {
    Modulo.loop();

    if (display.getButton(0)) {
        display.setTextColor(255,0,0);
    }
    if (display.getButton(1)) {
        display.setTextColor(0,255,0);
    }
    if (display.getButton(2)) {
        display.setTextColor(0,0,255);
    }

    display.clear();
    display.println("Hello Modulo!");
    display.setTextColor(display.White);
    display.println(millis()/1000);
    display.refresh();
}
