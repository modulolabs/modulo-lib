#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
DPadModule dpad;

void onButtonPress(DPadModule &module, uint8_t button) {
    for (int i=0; i < 5; i++) {
        Serial.print(module.getButton(i));
    }
    Serial.print(" ");
    Serial.print(button);
    Serial.println(" Pressed");
}

void onButtonRelease(DPadModule &module, uint8_t button) {
    for (int i=0; i < 5; i++) {
        Serial.print(module.getButton(i));
    }
    Serial.print(" ");
    Serial.print(button);
    Serial.println(" Released");
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    ModuloSetup();

    dpad.setButtonPressCallback(onButtonPress);
    dpad.setButtonReleaseCallback(onButtonRelease);
}

void loop() {
    ModuloLoop();

    uint8_t w = MiniDisplayModule::WIDTH;
    uint8_t h = MiniDisplayModule::HEIGHT;
    uint8_t size = 30;

    // put your main code here, to run repeatedly:
    display.setCursor(0,0);
    display.fillScreen(0);

    // Top
    display.drawTriangle(w/2, h/2,
                         w/2 + size, h/2 - size,
                         w/2 - size, h/2 - size,
                         WHITE);
    if (dpad.getButton(DPadModule::UP)) {
        display.fillTriangle(w/2, h/2,
                             w/2 + size, h/2 - size,
                             w/2 - size, h/2 - size,
                             WHITE);
    }

    // Bottom
    if (dpad.getButton(DPadModule::DOWN)) {
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
    if (dpad.getButton(DPadModule::RIGHT)) {
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
    if (dpad.getButton(DPadModule::LEFT)) {
        display.fillTriangle(w/2, h/2,
                             w/2 - size, h/2 - size,
                             w/2 - size, h/2 + size,
                             WHITE);
    }
    display.drawTriangle(w/2, h/2,
                         w/2 - size, h/2 - size,
                         w/2 - size, h/2 + size,
                         WHITE);
 

    display.fillCircle(w/2, h/2, size/2, dpad.getButton(DPadModule::CENTER) ? WHITE : BLACK);
    display.drawCircle(w/2, h/2, size/2, WHITE);    
    display.refresh();
}
