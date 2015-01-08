
#include "MiniDisplay.h"
#include "Wire.h"
        

MiniDisplay::MiniDisplay() : Adafruit_GFX(DISPLAY_WIDTH, DISPLAY_HEIGHT) {
}
  
void MiniDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    int16_t byteIndex = x + (y/8)*DISPLAY_WIDTH;
    int16_t bitIndex = y % 8;
    
    if (byteIndex < 0 or byteIndex >= DISPLAY_WIDTH*DISPLAY_HEIGHT/8) {
      return;
    }
    if (color) {
      _buffer[byteIndex] |= (1 << bitIndex);
    } else {
      _buffer[byteIndex] &= ~(1 << bitIndex);
    }
}

void MiniDisplay::fillScreen(uint16_t color) {
  if (color) {
    color = 0xFF;
  }
  for (int i=0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT/8; i++) {
      _buffer[i] = color;
  }
}

void MiniDisplay::update() {
    Wire.beginTransmission(10);
    Wire.write(3);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();


    for (int page=0; page < 8; page++) {
      Wire.beginTransmission(10);
      Wire.write(4);
      Wire.write(24);
      
      for (int x=0; x < DISPLAY_WIDTH; x++) {
      
        if (x % 24 == 23) {
          Wire.endTransmission();
          Wire.beginTransmission(10);
          Wire.write(4);
          Wire.write(24);
        }
        
        Wire.write(_buffer[page*DISPLAY_WIDTH + x]);
        //delay(100);
      }
      Wire.endTransmission();
    }

}


void MiniDisplay::SetStatus(bool status) {
    Wire.beginTransmission(10);
    Wire.write(207);
    Wire.write(status);
    Wire.endTransmission();
}

