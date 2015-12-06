#include "Modulo.h"
#if ARDUINO
#include "Wire.h"
#endif
#if SPARK
#include "math.h"
#define LED_BUILTIN 7
#endif

DisplayModulo display;

int page = 255;
bool serialConnected = false;
bool buttonWasPressed = false;

const uint8_t DELIMETER = 0x7E;
const uint8_t ESCAPE = 0x7D;

const uint8_t CODE_TRANSFER = 'T';
const uint8_t CODE_RETURN = 'R';
const uint8_t CODE_EXIT = 'Q';
const uint8_t CODE_ECHO = 'X';
const uint8_t CODE_DEBUG = 'D';
const uint8_t CODE_EVENT = 'V';

void sendPacket(uint8_t *data, uint8_t len) {
    Serial.write(DELIMETER);
    for (int i=0; i < len; i++) {
        if (data[i] == DELIMETER or data[i] == ESCAPE) {
            Serial.write(ESCAPE);
            Serial.write(data[i] ^ (1 << 5));
        } else {
            Serial.write(data[i]);
        }
    }
    Serial.write(DELIMETER);
    Serial.flush();
}

#define RECEIVE_PACKET_SIZE 64
uint8_t receivePacket[RECEIVE_PACKET_SIZE];
int receivePacketLen = 0;
bool receiveEscape = false;

void receiveData(int c) {
    if (c == DELIMETER) {
        if (receivePacketLen > 0) {
            processPacket(receivePacket, receivePacketLen);
            receivePacketLen = 0;
        }

        receiveEscape = false;
        return;
    }

    if (c == ESCAPE) {
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

    if (data[0] == CODE_TRANSFER) {
        processModuloTransfer(data+1, len-1);
    } else if (data[0] == CODE_EXIT) {
        serialConnected = false;
        Modulo.globalReset();
    } else if (data[0] == CODE_ECHO) {
        sendPacket(data, len);
    }
}

void debug(const char *s) {
    uint8_t packet[32];
    packet[0] = CODE_DEBUG;
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

    bool isString = (receiveLen == 31);

    bool retval = Modulo.transfer(address, command, buffer+4,
        sendLen, returnPacket+2, receiveLen, isString);

    returnPacket[0] = CODE_RETURN;
    returnPacket[1] = retval;

    if (retval) {
        sendPacket(returnPacket, receiveLen+2);
    } else {
        sendPacket(returnPacket, 2);
    }
}

uint16_t getDeviceIDByIndex(int index) {
    uint16_t deviceID = Modulo.getNextDeviceID(0);
    for (int i=0; i < index && deviceID != 0xFFFF; i++) {
        deviceID = Modulo.getNextDeviceID(deviceID+1);
    }

    return deviceID;
}


void showWelcomeScreen() {
    uint16_t deviceID = getDeviceIDByIndex(page);

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

        char deviceType[32] = {0};
        Modulo.getDeviceType(deviceID, deviceType, 31);
        if (strcmp(deviceType,"co.modulo.knob") == 0) {
            display.println("Knob");
        } else if (strcmp(deviceType,"co.modulo.blankslate") == 0) {
            display.println("Blank Slate");
        } else if (strcmp(deviceType,"co.modulo.joystick") == 0) {
            display.println("Joystick");
        } else if (strcmp(deviceType,"co.modulo.tempprobe") == 0) {
            display.println("Temp Probe");
        } else if (strcmp(deviceType,"co.modulo.display") == 0) {
            display.println("Display");
        } else if (strcmp(deviceType,"co.modulo.motor") == 0) {
            display.println("Motor Driver");
        } else if (strcmp(deviceType,"co.modulo.ir") == 0) {
            display.println("IR Remote");
        }

        display.println();
        display.print("ID: ");
        display.println(deviceID);

        display.setCursor(0, display.height()-8);
        display.println("          Next >");

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

void processEvent() {
    static const uint8_t BroadcastAddress = 9;
    static const uint8_t BroadcastCommandGetEvent = 7;
    static const uint8_t BroadcastCommandClearEvent = 8;

    uint8_t eventPacket[6] = {CODE_EVENT};
    if (Modulo.transfer(BroadcastAddress, BroadcastCommandGetEvent, 0, 0, eventPacket+1, 5)) {
        Modulo.transfer(BroadcastAddress, BroadcastCommandClearEvent, eventPacket+1, 5, 0, 0);

        sendPacket(eventPacket, 6);
    }
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
            uint8_t keepAlivePacket[] = {CODE_ECHO};
        }

        processEvent();

        if (Serial.available()) {
            receiveData(Serial.read());
        }
    } else {
        Modulo.loop();
        Modulo.exitBootloader();

        showWelcomeScreen();

        // Be careful with if (Serial)... it introduces a 10ms delay,
        // we can't check it while connected before every packet.
        if (Serial and Serial.available()) {
            serialConnected = true;
            display.clear();
            display.refresh();
        }
    }
}
