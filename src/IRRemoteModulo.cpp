#include "IRRemoteModulo.h"
#include "Modulo.h"
#include "ModuloWiring.h"
#include "IREncoding.h"
#include <math.h>

#define FUNCTION_RECEIVE 0
#define FUNCTION_GET_READ_SIZE 1
#define FUNCTION_CLEAR_READ 2
#define FUNCTION_SET_SEND_DATA 3
#define FUNCTION_SEND 4
#define FUNCTION_IS_IDLE 5
#define FUNCTION_SET_BREAK_LENGTH 6

#define EVENT_RECEIVE 0

IRRemoteModulo::IRRemoteModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.ir", deviceID)
{
}

IRRemoteModulo::IRRemoteModulo() :
    BaseModulo("co.modulo.ir")
{
}

void IRRemoteModulo::setBreakLength(uint16_t len) {
    uint8_t sendData[] = {static_cast<uint8_t>(len), static_cast<uint8_t>(len >> 8)};
    _transfer(FUNCTION_SET_BREAK_LENGTH, sendData, 2, 0, 0);
}

void IRRemoteModulo::_processEvent(uint8_t eventCode, uint16_t eventData) {
    uint8_t availableBytes = eventData;

    uint8_t data[128] = {0};

    for (uint8_t i=0; i < availableBytes and i < 128; i += 16) {
        uint8_t dataToSend[] = {i, 16};
        if (!_transfer(FUNCTION_RECEIVE, dataToSend, 2, data+i, 16)) {
            return;
        }
    }

    _transfer(FUNCTION_CLEAR_READ, 0, 0, 0, 0);

    uint16_t expandedData[_receiveBufferSize];
    uint16_t expandedLen = 0;
    for (int i=0; i <  availableBytes; i++) {
        uint16_t value = 0;
        if (data[i] == 0 and i+2 < availableBytes) {
            value = data[i+1] + (data[i+2] << 8);
            i += 2;
        } else {
            value = data[i];
        }

        // Due to sensor lag, each mark tends to be approx 1 tick long and
        // each space tends to be 1 tick short. Adjust for that here.
/*
        if (expandedLen%2) {
            value--;
        } else {
            value++;
        }
*/

        expandedData[expandedLen++] = value;

    }

    int8_t protocol = -1;
    uint32_t value = 0;
    IRDecode(expandedData, expandedLen, &protocol, &value);

    if (_receiveCallback) {
        _receiveCallback(protocol, value, expandedData, expandedLen);
    }
}

void IRRemoteModulo::send(uint8_t *data, uint8_t len)
{
    uint8_t isIdle = false;
    while (!isIdle) {
        if (!_transfer(FUNCTION_IS_IDLE, 0, 0, &isIdle, 1)) {
            return;
        }
        if (!isIdle) {
            delay(5);
        }
    }

    static const uint8_t packetSize = 16;
    uint8_t packet[packetSize+1];
    packet[0] = packetSize;

    for (int i=0; i < len; i += packetSize) {

        packet[0] = i;
        int packetLen = 1;
        for (int j=0; j < packetSize and (i+j) < len; j++) {
            packet[j+1] = data[i+j];
            packetLen++;
        }
        
        if (!_transfer(FUNCTION_SET_SEND_DATA, packet, packetLen, 0, 0)) {
            // Can't send. Bail.
            return;
        }
    }

    _transfer(FUNCTION_SEND, &len, 1, 0, 0);    
}

void IRRemoteModulo::setReceiveCallback(IRRemoteModulo::ReceiveCallback callback) {
    _receiveCallback = callback;
}
