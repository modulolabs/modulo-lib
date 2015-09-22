#include "IRRemoteModulo.h"
#include "Modulo.h"
#include <math.h>

#define FUNCTION_RECEIVE 0


IRRemoteModulo::IRRemoteModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.ir", deviceID)
{
}

IRRemoteModulo::IRRemoteModulo() :
    BaseModulo("co.modulo.ir")
{
}

bool IRRemoteModulo::receive(int8_t *protocol, uint32_t *code) {
    uint8_t receivedData[5];
    if (!_transfer(FUNCTION_RECEIVE, 0, 0, receivedData, 5)) {
        return false;
    }
    *protocol = static_cast<int8_t>(receivedData[0]);
    for (int i=0; i < 4; i++) {
        *code <<= 8;
        *code |= receivedData[i];
    }

    return true;
}
