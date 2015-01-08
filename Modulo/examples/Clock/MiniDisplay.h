#include <Adafruit_GFX.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64


class MiniDisplay : public Adafruit_GFX {

public:
  
  MiniDisplay();
  
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
  virtual void fillScreen(uint16_t color);
  
  void update();
  
   void SetStatus(bool status);
   
private :
  uint8_t _buffer[DISPLAY_WIDTH*DISPLAY_HEIGHT/8];
  
};

