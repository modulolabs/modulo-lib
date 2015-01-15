#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
DPadModule dpad;
  
void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    ModuloSetHighBitRate();
}

void loop() {
    delay(100);

    uint8_t w = MiniDisplayModule::WIDTH;
    uint8_t h = MiniDisplayModule::HEIGHT;
    uint8_t size = 30;

    // put your main code here, to run repeatedly:
    display.setCursor(0,0);
    display.fillScreen(0);

    uint8_t buttons = dpad.getButtons();
    // Top
    display.drawTriangle(w/2, h/2,
                         w/2 + size, h/2 - size,
                         w/2 - size, h/2 - size,
                         WHITE);
    if (buttons & _BV(1)) {
        display.fillTriangle(w/2, h/2,
                             w/2 + size, h/2 - size,
                             w/2 - size, h/2 - size,
                             WHITE);
    }

    // Bottom
    if (buttons & _BV(3)) {
        display.fillTriangle(w/2, h/2,
                             w/2 + size, h/2 + size,
                             w/2 - size, h/2 + size,
                             WHITE);
    }
    display.drawTriangle(w/2, h/2,
                         w/2 + size, h/2 + size,
                         w/2 - size, h/2 + size,
                         WHITE);

    // Right
    if (buttons & _BV(0)) {
        display.fillTriangle(w/2, h/2,
                             w/2 + size, h/2 - size,
                             w/2 + size, h/2 + size,
                             WHITE);
    }
    display.drawTriangle(w/2, h/2,
                         w/2 + size, h/2 - size,
                         w/2 + size, h/2 + size,
                         WHITE);

    // Left
    if (buttons & _BV(2)) {
        display.fillTriangle(w/2, h/2,
                             w/2 - size, h/2 - size,
                             w/2 - size, h/2 + size,
                             WHITE);
    }
    display.drawTriangle(w/2, h/2,
                         w/2 - size, h/2 - size,
                         w/2 - size, h/2 + size,
                         WHITE);
 

    display.fillCircle(w/2, h/2, size/2, (buttons & _BV(4)) ? WHITE : BLACK);
    display.drawCircle(w/2, h/2, size/2, WHITE);    

    display.println(dpad.getButtons(), BIN);
    display.display();
}
