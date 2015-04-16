#ifndef MODULO_COLOR_DISPLAY_H
#define MODULO_COLOR_DISPLAY_H

#include "Module.h"

#ifdef SPARK
#include "spark_wiring_print.h"
#else
#include "Print.h"
#endif
/// A tiny color OLED screen
class ColorDisplayModule : public Module, public Print {
public:
    virtual ~ColorDisplayModule();

    /// The width of the display
    static const int WIDTH = 96;

    /// The height of the display
    static const int HEIGHT = 64;

    enum TextAlignment {
        Left,
        Center,
        Right
    };

    struct Color {
        Color() : r(0), g(0), b(0), a(0) {}
        Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) :
            r(r_), g(g_), b(b_), a(a_) {}

        uint8_t r, g, b, a;
    };

    static const Color Black;
    static const Color White;
    static const Color Clear;

    /// Use the first MiniDisplay that's not already in use.
    ColorDisplayModule();

    /// Use the MiniDisplay with the specified deviceID
    ColorDisplayModule(uint16_t deviceID);

    void clear();
    
    //void setRotation(uint8_t r);
    //void setCursor(int x, int y);
    void setLineColor(const Color &c);
    void setFillColor(const Color &c);
    void setTextColor(const Color &textColor);
    //void setTextFormat(uint8_t size, TextAlignment alignment, bool wrap);


    void fillScreen(Color color);
    void setCursor(int x, int y);

    void drawLine(int x0, int y0, int x1, int y1);
    
    void drawRect(int x, int y, int w, int h, int radius=0);

    //void drawCircle(int x, int y, int r);
    
    //void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

    void drawString(const char *s);

    size_t write(uint8_t c);
    void refresh();

    bool isComplete();

    /// Return whether the specified button is currently pressed
    bool getButton(int button);

    /// Return the state of all three buttons, one in each bit.
    uint8_t getButtons();

    /// Draw the Modulo logo and the word 'MODULO' on a purple background
    void drawSplashScreen();

    /// Draw the Modulo logo
    void drawLogo(int x=0, int y=0, int width=49, int height=49);

private:
    void _waitOnRefresh();

    bool _isRefreshing;

};

#endif
