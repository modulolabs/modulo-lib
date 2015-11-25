#include "Modulo.h"
#include "Wire.h"

DisplayModulo display;
BlankSlateModulo io;
  
void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    for (int i=0; i < 8; i++) {
        //io.setPullup(i, true);
    }
}

void loop() {
    Modulo.loop();

    // put your main code here, to run repeatedly:
  
    //io.setPWMFrequency(5, 100);
    io.setPWMValue(4, .75);

    display.clear();


    for (int i=0; i < 8; i++) {
        if (i == 4) {
            continue;
        }
        display.print(i);
        display.print(" ");
        display.print(io.getDigitalInput(i));
        display.print(" ");
        display.print(io.getAnalogInput(i));
        display.println();        
    }


    display.refresh();

    delay(500);
}
