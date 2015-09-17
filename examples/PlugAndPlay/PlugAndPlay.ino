#include "Modulo.h"
#include "Wire.h"

KnobModulo knob;
JoystickModulo joystick;
DisplayModulo display;
TemperatureProbeModulo tempProbe;




bool knobWasPressed = false;
static const int numKnobColors = 6;
float hue = 0;
DisplayModulo::Color knobColors[numKnobColors] = {
    {255, 32, 32},
    {255, 255, 32},
    {32, 255, 32},
    {32, 255, 255},
    {32, 32, 255},
    {255, 32, 255}
};
int currentKnobColor = 0;

void drawKnobDisplay() {

    display.setLineColor(DisplayModulo::Color(255,255,255));

    bool knobIsPressed = knob.getButton();
    if (knobIsPressed and !knobWasPressed) {
        currentKnobColor = (currentKnobColor+1) % numKnobColors;
    }
    knobWasPressed = knobIsPressed;

    knob.setColor(knobColors[currentKnobColor].r/255.,
        knobColors[currentKnobColor].g/255.,
        knobColors[currentKnobColor].b/255.
        );

    display.setFillColor(knobColors[currentKnobColor]);
    display.drawCircle(display.width()/4, display.height()/3, 16);

    display.setLineColor(DisplayModulo::White);
    float angle = knob.getPosition()*M_PI/24.0;
    display.drawLine(display.width()/4, display.height()/3,
        display.width()/4 + 16*cos(angle), display.height()/3+16*sin(angle));
}


void drawDisplayDisplay() {
    display.setLineColor(DisplayModulo::Color(0,0,0,0));
    if (millis()/250 % 2) {
        display.setFillColor(DisplayModulo::Color(0,0,255));
    } else {
        display.setFillColor(DisplayModulo::Color(0,0,0));
    }
    display.drawRect(display.width()-10, 2, 6, 6);


    display.setLineColor(DisplayModulo::Color(0,128,255));
    float t = millis()/30.0;
    float prevY = -1;
    float prevX = 0;
    for (int x = 0; x < display.width() ; x+= 12) {
        float i = t + x;
        float y = 50 + 8*sin(i/10.0) + 2*sin(i/6.5);

        if (prevY != -1) {
            display.drawLine(prevX, prevY, x, y);
        }

        prevY = y;
        prevX = x;
    }

/*
    display.setLineColor(DisplayModulo::Color(0, 255, 255));
    prevY = -1;
    prevX = 0;
    for (int x = 0; x < display.width() ; x+= 4) {
        float i = t + x;
        float y = 50 + 8*sin(i/6.0) + 2*sin(i/3.5);

        if (prevY != -1) {
            display.drawLine(prevX, prevY, x, y);
        }

        prevY = y;
        prevX = x;
    }
    */

}

float tempProbeData[16];

void drawTempProbeDisplay() {
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(display.White);
    display.println(tempProbe.getTemperatureF());
}

void drawJoystickDisplay() {


    display.setLineColor(DisplayModulo::Color(255,255,255));
     
    if (joystick.getButton()) {
        display.setFillColor(DisplayModulo::Color(0,128,255));
        //DisplayModulo::Color(120, 0, 80));
    } else {
        display.setFillColor(display.Black);
    }

    display.drawCircle(
        display.width()*3/4 - joystick.getHPos()*10,
        display.height()/3 - joystick.getVPos()*10,
        10);


}

void setup() {

}

void loop() {
    Modulo.loop();

    display.clear();
    drawKnobDisplay();
    drawJoystickDisplay();
    drawDisplayDisplay();
    //drawTempProbeDisplay();
    display.refresh();
}