#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#ifdef SPARK
#include "spark_wiring_print.h"
#else
#include "Print.h"
#endif

#define swap(a, b) { int16_t t = a; a = b; b = t; }

class ModuloGFX : public Print {

 public:

  ModuloGFX(int16_t w, int16_t h); // Constructor

  /// Draw a pixel
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

  /// Draw a line
  virtual void
    drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

  /// Draw a rectangle
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

  /// Fill a rectangle
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  
  /// Fill the screen
  virtual void fillScreen(uint16_t color);

  /// Set invert display
  virtual void invertDisplay(bool i);

  /// Print a line of text, centered on the screen
  void printlnCentered(const char *text);

  /// Draw a circle
  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color);

  /// Fill a circle
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color);
  
  /// Draw a triangle
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);

  /// Fill a triangle
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color);
  
  /// Draw a rect with rounded corners
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);
  
  /// Fill a rect with rounded corners
  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
      int16_t radius, uint16_t color);

  /// Draw a bitmap
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t color);

  /// Draw a bitmap
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t color, uint16_t bg);

  /// Draw a bitmap
  void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, 
      int16_t w, int16_t h, uint16_t color);

  /// Draw a charachter
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size);

  /// Set the cursor position
  void setCursor(int16_t x, int16_t y);

  /// Set the text color
  void setTextColor(uint16_t c);
  
  /// Set the text color
  void setTextColor(uint16_t c, uint16_t bg);

  /// Set the text size
  void setTextSize(uint8_t s);
  
  /// Set text wrap
  void setTextWrap(bool w);

  /// Set rotation
  void setRotation(uint8_t r);

  /// Write a charachter
  virtual size_t write(uint8_t);

  /// Get the display height
  int16_t height(void) const;

  /// Get the display width
  int16_t width(void) const;

  /// Get the rotation
  uint8_t getRotation(void) const;

 protected:
  const int16_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  int16_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y;
  uint16_t
    textcolor, textbgcolor;
  uint8_t
    textsize,
    rotation;
  bool
    wrap; // If set, 'wrap' text at right edge of display
};

#endif // _ADAFRUIT_GFX_H
