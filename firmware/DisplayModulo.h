#ifndef MODULO_COLOR_DISPLAY_H
#define MODULO_COLOR_DISPLAY_H

#include "BaseModulo.h"

#ifdef SPARK
#include "spark_wiring_print.h"
#else
#include "Print.h"
#endif

/// A tiny color OLED screen
class DisplayModulo : public BaseModulo, public Print {
public:
    virtual ~DisplayModulo();

    int width() const {
        return WIDTH;
    }

    int height() const {
        return HEIGHT;
    }

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

    /// Use the first DisplayModulo that's not already in use.
    DisplayModulo();

    /// Use the DisplayModulo with the specified deviceID
    DisplayModulo(uint16_t deviceID);

    void clear();

    //void setRotation(uint8_t r);
    //void setCursor(int x, int y);
    void setLineColor(const Color &c);
    void setLineColor(uint8_t r, uint8_t g, uint8_t b) {
        setLineColor(Color(r,g,b));
    }

    void setFillColor(const Color &c);
    void setFillColor(uint8_t r, uint8_t g, uint8_t b) {
        setFillColor(Color(r,g,b));
    }

    void setTextColor(const Color &textColor);
    void setTextColor(uint8_t r, uint8_t g, uint8_t b) {
        setTextColor(Color(r,g,b));
    }

    void setTextSize(uint8_t size);

    void fillScreen(Color color);
    void fillScreen(uint8_t r, uint8_t g, uint8_t b) {
        fillScreen(Color(r,g,b));
    }

    void setCursor(int x, int y);

    void drawLine(int x0, int y0, int x1, int y1);

    void drawRect(int x, int y, int w, int h, int radius=0);

    void drawCircle(int x, int y, int r);

    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

    void drawString(const char *s);

    size_t write(uint8_t c);

    /// Display the results of all previous drawing commands.
    /// Note that after calling refresh, the next drawing operation will
    /// block until the frame has been drawn.
    void refresh();

    bool isComplete();

    bool isEmpty();

    /// Return whether the specified button is currently pressed
    bool getButton(int button);

    /// Return the state of all three buttons, one in each bit.
    uint8_t getButtons();

    /// Draw the Modulo logo and the word 'MODULO' on a purple background
    void drawSplashScreen();

    /// Set the brightness (Between 0 and 1).
    void setBrightness(float brightness);

    /// Draw the Modulo logo
    void drawLogo(int x=0, int y=0, int width=49, int height=49);

    void _beginOp(uint8_t opCode);
    void _appendToOp(uint8_t data);
    void _endOp();


    typedef void (EventCallback)(DisplayModulo &module, int button);

    void setButtonPressCallback(EventCallback *handler);
    void setButtonReleaseCallback(EventCallback *handler);

private:
    static const int OP_BUFFER_SIZE = 28;
    static const int WIDTH = 96;
    static const int HEIGHT = 64;

    EventCallback *_buttonPressCallback;
    EventCallback *_buttonReleaseCallback;
    uint8_t _buttonState;

    uint8_t _currentOp;
    uint8_t _opBuffer[OP_BUFFER_SIZE];
    uint8_t _opBufferLen;
    bool _isRefreshing;
    uint16_t _availableSpace;


    virtual void _processEvent(uint8_t eventCode, uint16_t eventData);

    // This sends raw data to the SSD1331 driver IC. Be very careful doing this,
    // since incorrect usage can damage/destroy the OLED.
    void _rawWrite(bool dataMode, uint8_t *data, size_t len);

    // Clip a line to be within the viewport. Returns false if the line
    // is entirely outside the viewport.
    bool _clipLine(int *x0, int *y0, int *x1, int *y1);
    
    // Helper for _clipLine
    int _computeOutCode(double x, double y);

    // Helper for drawString. Draws string s of length len (no null byte)
    void _drawString(const char *s, int len);

    // Wait until sufficient space is available in the modulo's op stream and
    // then append data to it.
    void _sendOp(uint8_t *data, uint8_t len);

    // This function is called at the beginning of every drawing operation.
    // If the last drawing operation was a refresh, it busy waits until
    // the modulo's drawing queue is empty. This provides a point of
    // syncronoization so that we don't queue drawing commands for more than
    // one frame at a time.
    void _waitOnRefresh();



};

#endif
