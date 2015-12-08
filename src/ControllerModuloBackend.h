#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "Modulo.h"

class _ControllerModuloBackend {
public:

    _ControllerModuloBackend();

    void loop();

    uint8_t getAddress();

    void globalReset();
    
    bool processTransfer(
        uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, bool receiveString);

    bool processBroadcastTransfer(
        uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, bool receiveString);

private:
    uint8_t _address;
    ModuloStatus _status;
};


#endif
