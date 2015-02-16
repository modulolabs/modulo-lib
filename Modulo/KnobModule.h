#ifndef KNOB_MODULE_H
#define KNOB_MODULE_H

#include "Module.h"

/// An illuminated Knob and push button
class KnobModule : public Module {

public:
    KnobModule();
    KnobModule(uint16_t deviceID);

    bool setColor(float r, float g, float b);
    bool setHSV(float h, float s, float v);
    
    bool getButton();
    int16_t getAngle();
    int16_t getPosition();
  
 private:
};

#endif
