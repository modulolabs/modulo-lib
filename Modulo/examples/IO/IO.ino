#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
IOModule io;
  
void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    for (int i=0; i < 8; i++) {
        //io.setPullup(i, true);
    }
}

void loop() {
    // put your main code here, to run repeatedly:
  
    display.setCursor(0,0);
    display.fillScreen(0);
    for (int i=0; i < 8; i++) {
        Serial.println(i);
        display.print(i);
        display.print(" ");
        io.setPullup(i, true);
        display.print(io.getDigitalInput(i));
        display.print(" ");
        io.setPullup(i, false);
        display.print(io.getAnalogInput(i));
        display.println();        
    }

    display.refresh();
}
