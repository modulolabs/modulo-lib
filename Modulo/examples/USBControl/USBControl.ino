#include "Modulo.h"
#include "Wire.h"
#include "Arduino.h"

#define BUFFER_SIZE 32


MiniDisplayModule display;

int page = 255;
bool serialConnected = false;
bool buttonWasPressed = false;

void processModuloTransfer() {
    uint8_t buffer[BUFFER_SIZE];

    if (Serial.readBytes(buffer, 3) != 3) {
        return;
    }

    uint8_t address = buffer[0];
    uint8_t command = buffer[1];
    uint8_t sendLen = buffer[2];

    if (Serial.readBytes(buffer, sendLen) != sendLen) {
        return;
    }

    uint8_t receiveLen = 0;
    Serial.readBytes(&receiveLen, 1);

    bool retval = moduloTransfer(address, command, buffer,
        sendLen, buffer, receiveLen);

    Serial.write(retval);
    if (retval and receiveLen > 0) {
        Serial.write(buffer, receiveLen);
    }
}

void drawFatLine(int x0, int y0, int x1, int y1, int thickness, int color=1) {

    int width = x1-x0+thickness;
    int height = y1-y0+thickness;

    x0 -= thickness/2;
    y0 -= thickness/2;

    if (height < 0) {
        y0 += height;
        height = -height;
    }

    display.fillRect(x0, y0, width, height, color);
}

void drawLogo(int x=0, int y=0, int width=49, int height=49) {

    int lineWidth = width/7;

    display.fillRect(x, y, width, lineWidth, 1);
    display.fillRect(x, y, lineWidth, height, 1);
    display.fillRect(x+width-lineWidth, y, lineWidth, height, 1);

    display.fillRect(x+lineWidth*2, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    display.fillRect(x+lineWidth*4, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    display.fillRect(x+lineWidth*2, y+height-lineWidth, lineWidth*3, lineWidth, 1);


}

void showWelcomeScreen() {
    uint16_t deviceID = ModuloGetNextDeviceID(0);
    for (int i=0; i < page && deviceID != 0xFFFF; i++) {
        deviceID = ModuloGetNextDeviceID(deviceID+1);
    }

    if (deviceID == 0xFFFF) {            
        display.clear();
        display.setCursor(0,40);
        display.setTextSize(1);
        display.printlnCentered("MODULO");

        drawLogo(display.WIDTH/2-18, 10, 35, 26);


        display.setTextSize(1);
        display.setCursor(0, display.height()-8);
        display.println("            Devices >");
        display.refresh();           
    } else {
        ModuloSetStatus(deviceID, ModuloStatusBlinking);

        display.clear();
        display.setCursor(0,0);
        display.setTextSize(1);

        char product[32];
        ModuloGetProduct(deviceID, product, 31);

        display.print("Device ID: ");
        display.println(deviceID);
        display.println();
        display.print("Type: ");    
        display.println(product);
        
        display.setCursor(0, display.height()-8);
        display.println("               Next >");

        display.refresh();
    }

    bool buttonIsPressed = display.getButton(2);
    if (buttonIsPressed and !buttonWasPressed) {
        if (deviceID != 0xFFFF) {
            ModuloSetStatus(deviceID, ModuloStatusOff);
            page++;
        } else {
            page = 0;
        }
    }
    buttonWasPressed = buttonIsPressed;
}

void setup() {
    ModuloSetup(true /* highBitRate*/);
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    ModuloLoop();

    if (serialConnected) {
        uint8_t command = Serial.read();
        if (command == 'T') {
            digitalWrite(LED_BUILTIN, true);
            processModuloTransfer();
            digitalWrite(LED_BUILTIN, false);
        } else if (command == 'E') {
            serialConnected = false;
            ModuloGlobalReset();
        }
    } else {
        showWelcomeScreen();

        // Be careful with if (Serial)... it introduces a 10ms delay,
        // we can't check it while connected before every packet.
        if (Serial and Serial.available()) {
            serialConnected = true;
        }
    }
}

