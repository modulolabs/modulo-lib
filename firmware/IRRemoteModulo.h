#ifndef IR_REMOTE_MODULO_H
#define IR_REMOTE_MODULO_H

#include "BaseModulo.h"

/// Infrared remote control transmitter and receiver
class IRRemoteModulo : public BaseModulo {

public:
    /// Use the first IRRemoteModulo that's not already in use
    IRRemoteModulo();

    /// Use the knob with the specified deviceID
    IRRemoteModulo(uint16_t deviceID);

    void update();

    /// Send raw data. Each byte is the number of 50us ticks that the output
    /// should be on or off. The first byte is an off period.
    void send(uint8_t *data, uint8_t len);

    /// Set the no signal time that's required before the receiver considers
    /// a transmission complete.
    void setBreakLength(uint16_t len);

    /// A callback function
    typedef void(*ReceiveCallback)(int8_t protocol, uint32_t value, uint16_t *rawData, uint8_t rawLen);
 
    /// Set the function that should be called when data is received
    void setReceiveCallback(ReceiveCallback callback);
private:

    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);

    static const uint8_t _receiveBufferSize = 128;
    uint8_t _receiveBuffer[_receiveBufferSize];
    uint16_t _receiveBufferLen;
    ReceiveCallback _receiveCallback;
};

#endif
