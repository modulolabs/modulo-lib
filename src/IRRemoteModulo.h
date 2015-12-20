#ifndef IR_REMOTE_MODULO_H
#define IR_REMOTE_MODULO_H

#include "BaseModulo.h"

/// Infrared remote control transmitter and receiver
///
/// This class supports encoding and decoding many of the most popular IR
/// remote control protocols, including NEC, Sony, JVC, Panasonic, and LG.
///
/// If a particular remote is not supported you can still receive raw IR data
/// (a sequence of pulse lengths). If you share the raw data for a particular
/// unsupported remote on the forum ( community.modulo.co ) we may be able to
/// add support for it. You can capture the raw data using the IRTutorial
/// example sketch.
class IRRemoteModulo : public BaseModulo {

public:
    /// Use the first IRRemoteModulo that's not already in use.
    IRRemoteModulo();

    /// Use the IRRemoteModulo with the specified deviceID.
    IRRemoteModulo(uint16_t deviceID);

    /// Send an IR code with the specified protocol number and data bits.
    void send(int8_t protocol, uint32_t data);

    /// Send raw data. Each byte is the number of 50us ticks that the output
    /// should be on or off. The first byte is an off period.
    void sendRaw(uint8_t *data, uint8_t len);

    /// Set the no signal time that's required before the receiver considers
    /// a transmission complete.
    void setBreakLength(uint16_t len);

    /// The callback function type
    typedef void(*ReceiveCallback)(int8_t protocol, uint32_t data,
        uint16_t *rawData, uint8_t rawLen);
 
    /// Set the function that should be called when data is received.
    /// The callback will receive the decoded protocol and data, along with
    /// an array containing the raw pulse widths.
    void setReceiveCallback(ReceiveCallback callback);
private:

    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);

    static const uint8_t _receiveBufferSize = 128;
    uint8_t _receiveBuffer[_receiveBufferSize];
    uint16_t _receiveBufferLen;
    ReceiveCallback _receiveCallback;
};

#endif
