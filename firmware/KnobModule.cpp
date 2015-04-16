#include "KnobModule.h"
#include "Modulo.h"
#include "Arduino.h"
#include <math.h>

#define FUNCTION_KNOB_GET_BUTTON 0
#define FUNCTION_KNOB_GET_POSITION 1
#define FUNCTION_KNOB_ADD_POSITION_OFFSET 2
#define FUNCTION_KNOB_SET_COLOR 3

KnobModule::KnobModule(uint16_t deviceID) :
    Module("co.modulo.knob", deviceID)
{
}

KnobModule::KnobModule() :
    Module("co.modulo.knob")
{
}


static void HSVToRGB(float h, float s, float v,
              float *r, float *g, float *b) {
    *r = *g = *b = 0;

    // Put h in the range [0, 6)
    h = 6*(h - floor(h));

    float C = v*s;
    float X = C*(1-fabs(fmod(h, 2) - 1));

    switch (int(h)) {
        case 0:
            *r = C;
            *g = X;
            break;
        case 1:
            *r = X;
            *g = C;
            break;
        case 2:
            *g = C;
            *b = X;
            break;
        case 3:
            *g = X;
            *b = C;
            break;
        case 4:
            *r = X;
            *b = C;
            break;
        case 5:
            *r = C;
            *b = X;
            break;
    }
    
    float m = v-C;

    *r += m;
    *g += m;
    *b += m;
}

bool KnobModule::setHSV(float h, float s, float v) {
    float r,g,b;
    HSVToRGB(h,s,v,&r,&g,&b);
    setColor(r, g, b);
}

bool KnobModule::setColor(float r, float g, float b) {
    uint8_t sendData[3] = {r*255, g*255, b*255};
    return _transfer(FUNCTION_KNOB_SET_COLOR, sendData, 3, 0, 0);
}

bool KnobModule::getButton() {
    uint8_t receivedData[1];
    if (!_transfer(FUNCTION_KNOB_GET_BUTTON, 0, 0, receivedData, 1)) {
        return false;
    }
    return receivedData[0];
}

int16_t KnobModule::getPosition() {
    uint8_t receivedData[2];
    if (!_transfer(FUNCTION_KNOB_GET_POSITION, 0, 0, receivedData, 2)) {
        Serial.println("Failed to get position");
        return false;
    }
    return receivedData[0] | (receivedData[1] << 8);
}

int16_t KnobModule::getAngle() {
    int16_t position = -getPosition();
    position = ((position % 24) + 24) % 24;
    return position*360/24;

}