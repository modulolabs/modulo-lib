#ifndef IR_REMOTE_MODULO_H
#define IR_REMOTE_MODULO_H

#include "BaseModulo.h"

/// An illuminated knob and push button
class IRRemoteModulo : public BaseModulo {

public:
    /// Use the first IRRemoteModulo that's not already in use
    IRRemoteModulo();

    /// Use the knob with the specified deviceID
    IRRemoteModulo(uint16_t deviceID);


    //uint8_t receive(uint8_t *buffer, uint8_t maxLen);

    void update();

    void send(uint8_t *data, uint8_t len);

    void setBreakLength(uint16_t len);

private:
    long _lastEmptyTime;

    static const uint8_t _receiveBufferSize = 128;
    uint8_t _receiveBuffer[_receiveBufferSize];
    uint16_t _receiveBufferLen;
};

#endif
