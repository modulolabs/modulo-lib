
// This class uses drawing code from the Adafruit SSD1306 library.
// 

/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
These displays use SPI to communicate, 4 or 5 pins are required to
interface
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/



#include <avr/pgmspace.h>
#include <stdlib.h>

#include <Wire.h>

#include "MiniDisplayModule.h"
#include "Modulo.h"

#define SET_PIXELS_COMMAND 0
#define GET_BUTTONS_COMMAND 1

// The Adafruit Spash Screen is required for redistribution of the Adafruit
// drawing code, but it takes up 1k of program space. Setting this to 0 will
// cause it to not be used, and the compiler will optimize it away.
#define USE_SPLASH_SCREEN 0

const uint8_t spashScreen[MiniDisplayModule::WIDTH*MiniDisplayModule::HEIGHT/8] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, \
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, \
    0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF, \
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, \
    0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8, \
    0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, \
    0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01, \
    0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF, \
    0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00, \
    0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF, \
    0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF, \
    0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F, \
    0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, \
    0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03, \
    0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, \
    0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00, \
    0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, \
    0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03, \
    0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F, \
    0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF, \
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F, \
    0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0, \
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, \
    0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E, \
    0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC, \
    0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06, \
    0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8, \
    0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, \
    0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C, \
    0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, \
    0x07, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


MiniDisplayModule::MiniDisplayModule() :
    ModuloGFX(MiniDisplayModule::WIDTH, MiniDisplayModule::HEIGHT),
    Module("co.modulo.MiniDisplay"),
    _forceRedisplay(true),
    _lastForcedBlock(0)
{
    _init();
}

MiniDisplayModule::MiniDisplayModule(uint16_t deviceID) :
    ModuloGFX(MiniDisplayModule::WIDTH, MiniDisplayModule::HEIGHT),
    Module("co.modulo.MiniDisplay", deviceID),
    _forceRedisplay(true),
    _lastForcedBlock(0)
{
    _init();
}

void MiniDisplayModule::_init() {
    setTextColor(WHITE, BLACK);
    
    for (int i=0; i < WIDTH*HEIGHT/8 ; i++) {
#if USE_SPLASH_SCREEN
        _buffer[i] = pgm_read_byte(&(spashScreen[i]));
#endif
    }
}


void MiniDisplayModule::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }  

  // x is which column
  switch (color)  {
  case WHITE:
      _buffer[x+ (y/8)*MiniDisplayModule::WIDTH] |=  (1 << (y&7));
      break;
  case BLACK:
      _buffer[x+ (y/8)*MiniDisplayModule::WIDTH] &= ~(1 << (y&7));
      break; 
  case INVERSE:
      _buffer[x+ (y/8)*MiniDisplayModule::WIDTH] ^=  (1 << (y&7));
      break; 
  }
    
}


bool MiniDisplayModule::getButton(int button) {
    return getButtons() & _BV(button);
}

uint8_t MiniDisplayModule::getButtons() {
    uint8_t receivedData[1] = {0};
    if (!moduloTransfer(getAddress(), GET_BUTTONS_COMMAND, 0, 0, receivedData, 1)) {
        return false;
    }
    return receivedData[0];
}


void MiniDisplayModule::invertDisplay(uint8_t i) {
    // XXX: unimplemented
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void MiniDisplayModule::startscrollright(uint8_t start, uint8_t stop){
    // XXX: unimplemented
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void MiniDisplayModule::startscrollleft(uint8_t start, uint8_t stop){
    // XXX: unimplemented
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void MiniDisplayModule::startscrolldiagright(uint8_t start, uint8_t stop){
    // XXX: unimplemented
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void MiniDisplayModule::startscrolldiagleft(uint8_t start, uint8_t stop){
    // XXX: unimplemented
}

void MiniDisplayModule::stopscroll(void){
    // XXX: unimplemented
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void MiniDisplayModule::dim(uint8_t dim) {
    // XXX: unimplemented
}

uint8_t _crc8_ccitt_update (uint8_t inCrc, uint8_t inData);

void MiniDisplayModule::display() {
    // Data is sent to the display module in blocks that are 8 pixels high (1 page) and
    // 16 pixels wide. To reduce bandwidth, we only send the blocks that have changed.
    // We detect changes using a CRC of each block's pixels, which lets us avoid the RAM
    // cost of an entire second framebuffer.
    //
    // Since the pixels can sometimes be different even when the CRCs match, we also
    // force transmission of one block at each refresh, so that after 64 refreshes we
    // are guaranteed to have pushed all blocks.

    uint8_t buffer[18];
    uint8_t hashIndex = 0;
    _lastForcedBlock = (_lastForcedBlock+1)%64;
    for (int page=0; page < 8; page++) {
        for (int x=0; x < WIDTH; x += 16) {
            buffer[0] = page;
            buffer[1] = x;
            
            uint8_t newHash = 0;
            for (int i=0; i < 16; i++) {
                newHash = _crc8_ccitt_update(newHash, _buffer[page*WIDTH + x + i]);
            }
            
            if (_forceRedisplay or hashIndex == _lastForcedBlock or
                newHash != _hashes[hashIndex]) {
                memcpy(buffer+2, &_buffer[page*WIDTH + x], 16);
                moduloTransfer(getAddress(), SET_PIXELS_COMMAND, buffer, 18, 0, 0);
            }
            _hashes[hashIndex] = newHash;
            hashIndex++;
        }
    }

    _forceRedisplay = false;
}

// clear everything
void MiniDisplayModule::clearDisplay(void) {
    memset(_buffer, 0, (MiniDisplayModule::WIDTH*MiniDisplayModule::HEIGHT/8));
}


void MiniDisplayModule::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  boolean bSwap = false;
  switch(rotation) { 
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }
  
  if(bSwap) { 
      drawFastVLineInternal(x, y, w, color);
  } else { 
      drawFastHLineInternal(x, y, w, color);
  }
}

void MiniDisplayModule::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Do bounds/limit checks
    if(y < 0 || y >= HEIGHT) { return; }

    // make sure we don't try to draw below 0
    if(x < 0) { 
        w += x;
        x = 0;
    }
    
    // make sure we don't go off the edge of the display
    if( (x + w) > WIDTH) { 
        w = (WIDTH - x);
    }

    // if our width is now negative, punt
    if(w <= 0) { return; }

    // set up the pointer for  movement through the buffer
    register uint8_t *pBuf = _buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y/8) * MiniDisplayModule::WIDTH);
    // and offset x columns in
    pBuf += x;

    register uint8_t mask = 1 << (y&7);

    switch (color)  {
    case WHITE:
        while(w--) {
            *pBuf++ |= mask;
        };
        break;
    case BLACK:
        mask = ~mask;
        while(w--) {
            *pBuf++ &= mask;
        };
        break;
    case INVERSE:
        while(w--) {
            *pBuf++ ^= mask;
        };
        break;
    }
}

void MiniDisplayModule::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    bool bSwap = false;
    switch(rotation) { 
    case 0:
        break;
    case 1:
        // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
        bSwap = true;
        swap(x, y);
        x = WIDTH - x - 1;
        x -= (h-1);
        break;
    case 2:
        // 180 degree rotation, invert x and y - then shift y around for height.
        x = WIDTH - x - 1;
        y = HEIGHT - y - 1;
        y -= (h-1);
        break;
    case 3:
        // 270 degree rotation, swap x & y for rotation, then invert y 
        bSwap = true;
        swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    if(bSwap) { 
        drawFastHLineInternal(x, y, h, color);
    } else {
        drawFastVLineInternal(x, y, h, color);
    }
}


void MiniDisplayModule::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {
    // do nothing if we're off the left or right side of the screen
    if(x < 0 || x >= WIDTH) {
        return;
    }
    
    // make sure we don't try to draw below 0
    if(__y < 0) { 
        // __y is negative, this will subtract enough from __h to account for __y being 0
        __h += __y;
        __y = 0;
    } 
    
    // make sure we don't go past the height of the display
    if( (__y + __h) > HEIGHT) { 
        __h = (HEIGHT - __y);
    }

    // if our height is now negative, punt 
    if(__h <= 0) { 
        return;
    }

    // this display doesn't need ints for coordinates, use local byte registers for faster juggling
    register uint8_t y = __y;
    register uint8_t h = __h;


    // set up the pointer for fast movement through the buffer
    register uint8_t *pBuf = _buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y/8) * MiniDisplayModule::WIDTH);
    // and offset x columns in
    pBuf += x;

    // do the first partial byte, if necessary - this requires some masking
    register uint8_t mod = (y&7);
    if(mod) {
        // mask off the high n bits we want to set 
        mod = 8-mod;

        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        // register uint8_t mask = ~(0xFF >> (mod));
        static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        register uint8_t mask = premask[mod];

        // adjust the mask if we're not going to reach the end of this byte
        if( h < mod) { 
            mask &= (0XFF >> (mod-h));
        }

        switch (color) {
        case WHITE:
            *pBuf |=  mask; 
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^=  mask;
            break;
        }
  
        // fast exit if we're done here!
        if(h<mod) {
            return;
        }

        h -= mod;

        pBuf += MiniDisplayModule::WIDTH;
    }


    // write solid bytes while we can - effectively doing 8 rows at a time
    if(h >= 8) { 
        // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
        if (color == INVERSE)  {
            do  {
                *pBuf =~ (*pBuf);

                // adjust the buffer forward 8 rows worth of data
                pBuf += MiniDisplayModule::WIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            } while(h >= 8);
        }
        else {
            // store a local value to work with 
            register uint8_t val = (color == WHITE) ? 255 : 0;

            do  {
                // write our value in
                *pBuf = val;

                // adjust the buffer forward 8 rows worth of data
                pBuf += MiniDisplayModule::WIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            } while(h >= 8);
        }
    }

    // now do the final partial byte, if necessary
    if(h) {
        mod = h & 7;
        // this time we want to mask the low bits of the byte, vs the high bits we did above
        // register uint8_t mask = (1 << mod) - 1;
        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
        register uint8_t mask = postmask[mod];
        switch (color)  {
        case WHITE:
            *pBuf |=  mask;
            break;
        case BLACK:
            *pBuf &= ~mask;
            break;
        case INVERSE:
            *pBuf ^=  mask;
            break;
        }
    }
}
