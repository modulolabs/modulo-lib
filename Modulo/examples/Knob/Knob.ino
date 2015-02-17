#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
KnobModule knob;

int32_t oldKnobPosition = 0;
float sliderValue = 0;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
}

void loop() {
    int32_t newKnobPosition = knob.getPosition();

    sliderValue = sliderValue + 5*(newKnobPosition-oldKnobPosition);
    sliderValue = max(0, sliderValue);
    sliderValue = min(100, sliderValue);
    oldKnobPosition = newKnobPosition;


    float angle = knob.getAngle();

    display.setCursor(0,0);
    display.fillScreen(0);

    if (knob.getButton()) {
        knob.setColor(1,1,1);
    } else {
        knob.setHSV(knob.getAngle()/360.0, 1, 1);
    }


    // Draw the angle indicator
    int lineLength = 30;
    float x = display.width()/3;
    float y = display.height()/2;
    display.drawLine(x, y,
        x+lineLength*cos(angle*M_PI/180),
        y-lineLength*sin(angle*M_PI/180), WHITE);
    display.fillCircle(x, y, 3, knob.getButton());
    display.drawCircle(x, y, 3, WHITE);
    display.setCursor(0, 0);
    display.print(knob.getAngle());
    display.print(char(247));


    display.setCursor(display.width()-20, 0);
    display.print(int(sliderValue));
    display.drawRect(display.width()-20, 10, 20, display.height()-10, WHITE);
    display.fillRect(display.width()-20, 10+(display.height()-10)*(1-sliderValue/100.0), 20, display.height()-10, WHITE);
    display.display();

}

