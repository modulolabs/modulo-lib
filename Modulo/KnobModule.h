#ifndef KNOB_MODULE_H
#define KNOB_MODULE_H

#include "Module.h"

/// An illuminated Knob and push button
class KnobModule : public Module {

public:
    /// Use the first Knob that's not already in use
    KnobModule();

    /// Use the knob with the specified deviceID
    KnobModule(uint16_t deviceID);

    /// Set the color of the knob's LED in rgb.
    bool setColor(float r, float g, float b);

    /// Set the color of the knob's LED in hsv.
    bool setHSV(float h, float s, float v);
    
    /// Return whether the knob is currently being pressed
    bool getButton();

    /// Return the angle of the knob in degrees.
    int16_t getAngle();

    /// Return the position of the knob in clicks (24 per revolution).
    int16_t getPosition();
  
private:
};

#endif
