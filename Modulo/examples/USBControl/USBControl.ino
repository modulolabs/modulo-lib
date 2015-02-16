#include "Modulo.h"
#include "Wire.h"
#include "Arduino.h"

#define BUFFER_SIZE 32


void setup() {
    Serial.begin(9600);
    Wire.begin();
    pinMode(LED_BUILTIN, OUTPUT);
}

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

MiniDisplayModule display;

int page = 255;
bool serialConnected = false;
bool buttonWasPressed = false;

void loop() {
    if (Serial and !serialConnected) {
        display.clearDisplay();
        display.setCursor(0, display.height()/2);
        display.println("Serial Port Connected");
        display.display();
        serialConnected = false;
    }

    if (Serial) {
        serialConnected = true;
        page = 255;
    } else {
        uint16_t deviceID = ModuloGetNextDeviceID(0);
        for (int i=0; i < page && deviceID != 0xFFFF; i++) {
            deviceID = ModuloGetNextDeviceID(deviceID+1);
        }

        if (deviceID == 0xFFFF) {            
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.println(" Welcome");
            display.println("  to");
            display.println("   Modulo!");
            display.setTextSize(1);
            display.setCursor(0, display.height()-8);
            display.println("               Next >");
            display.display();           
        } else {
            ModuloSetStatus(deviceID, ModuloStatusBlinking);

            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);

            char product[32];
            ModuloGetProduct(deviceID, product, 31);

            display.print("Device ID: ");
            display.println(deviceID);
            display.println();
            display.print("Type: ");    
            display.println(product);

            

            display.display();
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

    if (Serial.available() && Serial.read() == 'T') {
        processModuloTransfer();
    }
}

