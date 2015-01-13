#ifndef MODULO_MINI_DISPLAY_H
#define MODULO_MINI_DISPLAY_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#include "ModuloGFX.h"
#include "ModuloBase.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2


class ModuloMiniDisplay : public ModuloGFX, public ModBase {
public:
    static const int WIDTH = 128;
    static const int HEIGHT = 64;

    ModuloMiniDisplay();
    ModuloMiniDisplay(uint16_t address);

    void clearDisplay(void);
    void invertDisplay(uint8_t i);
    void display();

    void startscrollright(uint8_t start, uint8_t stop);
    void startscrollleft(uint8_t start, uint8_t stop);

    void startscrolldiagright(uint8_t start, uint8_t stop);
    void startscrolldiagleft(uint8_t start, uint8_t stop);
    void stopscroll(void);

    void dim(uint8_t contrast);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 private:
    uint8_t _buffer[WIDTH*HEIGHT/8];
    
    inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
    inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
};

#endif
