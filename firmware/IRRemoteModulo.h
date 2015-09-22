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


    bool receive(int8_t *protocol, uint32_t *code);

private:
};

#endif
