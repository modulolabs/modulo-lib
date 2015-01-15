#ifndef KNOB_MODULE_H
#define KNOB_MODULE_H

#include "Module.h"

/// An illuminated Knob and push button
class KnobModule : public Module {

public:
    KnobModule();
    KnobModule(uint16_t deviceID);

    bool setColor(uint8_t r, uint8_t g, uint8_t b);
    
    bool getButton();
    int16_t getPosition();
  
 private:
};

#endif
