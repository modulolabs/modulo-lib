#ifndef MODULO_MINI_DISPLAY_H
#define MODULO_MINI_DISPLAY_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#include "Module.h"
#include "ModuloGFX.h"


#define BLACK 0
#define WHITE 1
#define INVERSE 2

/// A tiny OLED screen
class MiniDisplayModule : public Module, public ModuloGFX {
public:
    /// The width of the display
    static const int WIDTH = 128;

    /// The height of the display
    static const int HEIGHT = 64;

    /// Use the first MiniDisplay that's not already in use.
    MiniDisplayModule();

    /// Use the MiniDisplay with the specified deviceID
    MiniDisplayModule(uint16_t deviceID);

    /// Clear the display, setting all pixels to black.
    void clearDisplay(void);

    /// Update the MiniDisplay with the current frame buffer.
    void display();

    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

    /// Return whether the specified button is currently pressed
    bool getButton(int button);

    /// Return the state of all three buttons, one in each bit.
    uint8_t getButtons();

 private:
    uint8_t _buffer[WIDTH*HEIGHT/8]; // The actual pixel data
    uint8_t _hashes[64];             // The CRCs of the pixels from the last refresh
    bool _forceRedisplay;
    uint8_t _lastForcedBlock;

    void _init();
    
    inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
    inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
};

#endif
