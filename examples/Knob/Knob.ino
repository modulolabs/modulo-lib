#include "Modulo.h"
#include "Wire.h"

DisplayModulo display;
KnobModulo knob;

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

    display.clear();

    if (knob.getButton()) {
        knob.setColor(1,1,1);
    } else {
        knob.setHSV(knob.getAngle()/360.0, 1, 1);
    }


    // Draw the angle indicator
    int lineLength = 30;
    float x = display.width()/3;
    float y = display.height()/2;
    display.setLineColor(DisplayModulo::White);
    display.drawLine(x, y,
        x+lineLength*cos(angle*M_PI/180),
        y-lineLength*sin(angle*M_PI/180));

    
    display.drawCircle(x, y, 10);

    display.setCursor(0, 0);
    display.print(knob.getAngle());
    display.print(char(247));


    display.setCursor(display.width()-20, 0);
    display.print(int(sliderValue));
    display.setFillColor(DisplayModulo::White);
    display.drawRect(display.width()-20, 10, 20, display.height()-10);
    display.drawRect(display.width()-20, 10+(display.height()-10)*(1-sliderValue/100.0), 20, display.height()-10);
    display.refresh();

}
