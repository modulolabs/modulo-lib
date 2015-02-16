#include "DPadModule.h"
#include "Modulo.h"


#define DPAD_FUNCTION_GET_BUTTONS 0

DPadModule::DPadModule() : Module("co.modulo.dpad") {}

DPadModule::DPadModule(uint16_t deviceID) :
    Module("co.modulo.dpad", deviceID) {
}


bool DPadModule::getButton(uint8_t button) {
    return getButtons() & _BV(button);
}

bool DPadModule::getButton(Button button) {
    return getButton((uint8_t)button);
}

uint8_t DPadModule::getButtons() {
    uint8_t receivedData[1] = {0};
    if (!moduloTransfer(getAddress(), DPAD_FUNCTION_GET_BUTTONS, 0, 0, receivedData, 1)) {
        return false;
    }
    return receivedData[0];
}
