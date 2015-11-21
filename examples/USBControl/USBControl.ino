#include "Modulo.h"
#include "Wire.h"
#include "Arduino.h"

DisplayModulo display;

int page = 255;
bool serialConnected = false;
bool buttonWasPressed = false;

const uint8_t delimeter = 0x7E;
const uint8_t escape = 0x7D;

void sendPacket(uint8_t *data, uint8_t len) {
    //digitalWrite(LED_BUILTIN, true);

    Serial.write(delimeter);
    for (int i=0; i < len; i++) {
        if (data[i] == delimeter or data[i] == escape) {
            Serial.write(escape);
            Serial.write(data[i] ^ (1 << 5));
        } else {
            Serial.write(data[i]);
        }
    }
    Serial.write(delimeter);
    Serial.flush();
}

#define RECEIVE_PACKET_SIZE 64
uint8_t receivePacket[RECEIVE_PACKET_SIZE];
int receivePacketLen = 0;
bool receiveEscape = false;

void receiveData(int c) {
    if (c == delimeter) {
        if (receivePacketLen > 0) {
            processPacket(receivePacket, receivePacketLen);
            receivePacketLen = 0;
        }

        receiveEscape = false;
        return;
    }

    if (c == escape) {
        receiveEscape = true;
        return;
    }

    if (receiveEscape) {
        c ^= (1 << 5);
        receiveEscape = false;
    }

    if (receivePacketLen < RECEIVE_PACKET_SIZE) {
        receivePacket[receivePacketLen++] = c;
    }
}

void processPacket(uint8_t *data, uint8_t len) {

    if (len < 1) {
        return;
    }

    if (data[0] == 'T') {
        processModuloTransfer(data+1, len-1);
        //digitalWrite(LED_BUILTIN, false);

        return;

    } else if (data[0] == 'E') {
        serialConnected = false;
        Modulo.globalReset();
    } else if (data[0] == 'P') {
        Serial.write("Hello\n");
    } else if (data[0] == 'X') {
        sendPacket(data, len);
    }
}

void debug(const char *s) {
    uint8_t packet[32];
    packet[0] = 'D';
    int len = 1;
    while (*s) {
        packet[len++] = *(s++);
    }
    sendPacket(packet, len);
}

void processModuloTransfer(uint8_t *buffer, int len) {
    if (len < 4) {
        return;
    }

    digitalWrite(LED_BUILTIN, true);


    uint8_t address = buffer[0];
    uint8_t command = buffer[1];
    uint8_t sendLen = buffer[2];
    uint8_t receiveLen = buffer[3];

    if (len != sendLen+4) {
        debug("Length match error");
        return;
    }

    // Return packet contains:
    //    Packet code ('R')
    //    Return status (0 or 1)
    //    Returned data
    uint8_t returnPacket[64] = {0};

    bool retval = Modulo.transfer(address, command, buffer+4,
        sendLen, returnPacket+2, receiveLen);

    returnPacket[0] = 'R';
    returnPacket[1] = retval;

    if (retval) {
        sendPacket(returnPacket, receiveLen+2);
    } else {
        sendPacket(returnPacket, 2);
    }

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

long lastKeepAlive = 0;

void loop() {


    if (serialConnected) {
        if (millis() > lastKeepAlive+100) {
            lastKeepAlive = millis();
            uint8_t keepAlivePacket[] = {'X'};

        }
        if (Serial.available()) {
            receiveData(Serial.read());
        }
    } else {
        Modulo.loop();

         showWelcomeScreen();
 
         // Be careful with if (Serial)... it introduces a 10ms delay,
         // we can't check it while connected before every packet.
         if (Serial and Serial.available()) {
            serialConnected = true;
            display.clear();
            display.refresh();
            delay(100);
         }
     }
}

