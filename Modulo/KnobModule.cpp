#include "KnobModule.h"
#include "Modulo.h"

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

bool KnobModule::setColor(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t sendData[3] = {r, g, b};
    return moduloTransfer(getAddress(), FUNCTION_KNOB_SET_COLOR, sendData, 3, 0, 0);
}

bool KnobModule::getButton() {
    uint8_t receivedData[1];
    if (!moduloTransfer(getAddress(), FUNCTION_KNOB_GET_BUTTON, 0, 0, receivedData, 1)) {
        return false;
    }
    return receivedData[0];
}

int16_t KnobModule::getPosition() {
    uint8_t receivedData[2];
    if (!moduloTransfer(getAddress(), FUNCTION_KNOB_GET_POSITION, 0, 0, receivedData, 2)) {
        return false;
    }
    return receivedData[0] | (receivedData[1] << 8);
}
