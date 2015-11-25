#include "Modulo.h"
#include "Wire.h"

DisplayModulo display;

void buttonPressCallback(DisplayModulo &, int i) {
    Serial.print(i);
    Serial.println(" pressed");
}

void buttonReleaseCallback(DisplayModulo &, int i) {
    Serial.print(i);
    Serial.println(" released");
}

void setup() {
    Modulo.setup();
    Serial.begin(9600);

    display.setButtonPressCallback(buttonPressCallback);
    display.setButtonReleaseCallback(buttonReleaseCallback);
}

long lastDrawStart = 0;

void drawGraph(int y0, int amplitude, float t, float freq, float duration) {
    int dx = display.width()/30;
    int prevX = -1;
    int prevY = -1;
    for (int x=0; x <= display.width(); x += dx) {
        // t0 is the time at the current pixel
        float t0 = t + duration*float(x)/display.width();
        int y = y0 + amplitude*sin(2*M_PI*t0*freq);
        if (prevX != -1) {
            display.drawLine( prevX, prevY, x, y);
        }
        prevX = x;
        prevY = y;
    }
}



void loop() {
    Modulo.loop();
    long newDrawStart = millis();
    float fps = 1000.0/(newDrawStart-lastDrawStart);
    display.clear();
    display.setFillColor(0,0,255);
    display.setLineColor(255,0,0);
    display.setCursor(0, 40);
    display.println(fps);

    //display.setBrightness((millis() % 1000)/1000.);
/*
    int x = 123; // + ((millis()/500) % 2);
    display.print(x); display.print(x); display.print(x); display.println(x);
    display.print(x); display.print(x); display.print(x); display.println(x);
    display.print(x); display.print(x); display.print(x); display.println(x);
    display.print(x); display.print(x); display.print(x); display.println(x);
    display.print(x); display.print(x); display.print(x); display.println(x);
    display.print(x); display.print(x); display.print(x); display.println(x);
*/

    //display.drawLine(10,10,20,20);



    drawGraph(display.height()/3, 10, newDrawStart/1000.0, 1, 1);
    drawGraph(display.height()*2/3, 10, newDrawStart/1000.0, 2, 1);


    //display.drawRect(-300,-40, 80, 80, 20);

/*
    display.drawTriangle( 15, 10,
                          10, 50,
                          50, 20);
*/

    display.refresh();



    lastDrawStart = newDrawStart;
}