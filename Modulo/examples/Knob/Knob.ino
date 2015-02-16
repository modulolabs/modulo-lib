#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
KnobModule knob;
  
void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
}

uint16_t frameDuration;

void loop() {
    // put your main code here, to run repeatedly:
    Serial.println("loop");

    display.setCursor(0,0);
    display.fillScreen(0);
    display.println(knob.getButton());
    display.println(knob.getPosition());
    display.println(frameDuration);

    int32_t pos = knob.getPosition();
    if (knob.getButton()) {
       knob.setColor(255,0,0);
    } else {
        knob.setHSV((pos % 24)/24.0, 1, 1);
//       knob.setColor(255,(pos % 24)*10,255-((pos % 24)*10));
    }

    float angle = (pos % 24)*2*M_PI/24.0;
    int len = 30;
    display.drawLine(display.width()/2,
                     display.height()/2,
                     display.width()/2 + round(len*cos(angle)),
                     display.height()/2 + round(len*sin(angle)), WHITE);

    frameDuration = -millis();
    display.display();
    frameDuration += millis();
}

