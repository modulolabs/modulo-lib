#include "Modulo.h"
#include "Wire.h"
#include "Arduino.h"

#define BUFFER_SIZE 32

DisplayModulo display;

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

    bool retval = Modulo.transfer(address, command, buffer,
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

    display.drawRect(x0, y0, width, height, color);
}

KnobModulo knob;

void drawKnobDisplay(uint16_t deviceID) {


    display.println(knob.getPosition());
    display.setLineColor(display.White);
    float angle = knob.getPosition()*M_PI/24.0;
    display.drawLine(display.width()/2, display.height()/2,
        display.width()/2 + 10*cos(angle), display.height()/2+10*sin(angle));
}


void showWelcomeScreen() {
    uint16_t deviceID = Modulo.getNextDeviceID(0);
    for (int i=0; i < page && deviceID != 0xFFFF; i++) {
        deviceID = Modulo.getNextDeviceID(deviceID+1);
    }

    if (deviceID == 0xFFFF) {            
        display.clear();
     
        display.drawSplashScreen();


        display.setCursor(0, display.height()-8);
        display.println("       Devices >");
        display.refresh();           
    } else {
        Modulo.setStatus(deviceID, ModuloStatusBlinking);

        display.clear();
        display.setCursor(0,0);

        char product[32];
        Modulo.getProduct(deviceID, product, 31);

        display.print(product);
        display.setCursor(66, 0);
        display.println(deviceID);

        char deviceType[32];
        Modulo.getDeviceType(deviceID, deviceType, 31);
        //if (strcmp(deviceType,"co.modulo.knob") == 0) {
            drawKnobDisplay(deviceID);
        //}

        
        display.setCursor(0, display.height()-8);
        display.println("               Next >");

        display.refresh();
    }

    bool buttonIsPressed = display.getButton(2);
    if (buttonIsPressed and !buttonWasPressed) {
        if (deviceID != 0xFFFF) {
            Modulo.setStatus(deviceID, ModuloStatusOff);
            page++;
        } else {
            page = 0;
        }
    }
    buttonWasPressed = buttonIsPressed;
}

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    Modulo.loop();

    if (serialConnected) {
        uint8_t command = Serial.read();
        if (command == 'T') {
            digitalWrite(LED_BUILTIN, true);
            processModuloTransfer();
            digitalWrite(LED_BUILTIN, false);
        } else if (command == 'E') {
            serialConnected = false;
            Modulo.globalReset();
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

