#ifndef KNOB_MODULE_H
#define KNOB_MODULE_H

#include "Module.h"

/// An illuminated knob and push button
class KnobModulo : public ModuloBase {

public:
    /// Use the first KnobModulo that's not already in use
    KnobModulo();

    /// Use the knob with the specified deviceID
    KnobModulo(uint16_t deviceID);

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
