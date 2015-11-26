#ifndef MODULO_COLOR_DISPLAY_H
#define MODULO_COLOR_DISPLAY_H

#include "BaseModulo.h"

#ifdef SPARK
#include "spark_wiring_print.h"
#else
#include "Print.h"
#endif

/// A full color OLED display.
class DisplayModulo : public BaseModulo, public Print {
public:
    virtual ~DisplayModulo();

    /// Return the width of the display in pixels
    int width() const {
        return WIDTH;
    }

    /// Return the height of the display in pixels
    int height() const {
        return HEIGHT;
    }

    enum TextAlignment {
        Left,
        Center,
        Right
    };

    /// An RGBA color
    struct Color {
        /// Construct a new color with all channels 0. (clear)
        Color() : r(0), g(0), b(0), a(0) {}

        /// Construct a new color with the specified values
        Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255) :
            r(r_), g(g_), b(b_), a(a_) {}

        /// The red channel
        uint8_t r;

        /// The green channel
        uint8_t g;

        /// The blue channel
        uint8_t b;

        /// The alpha channel
        uint8_t a;
    };

    /// Black
    static const Color Black;

    /// White
    static const Color White;

    /// Clear
    static const Color Clear;

    /// Use the first DisplayModulo that's not already in use.
    DisplayModulo();

    /// Use the DisplayModulo with the specified deviceID
    DisplayModulo(uint16_t deviceID);

    /// Fill the screen with black and return the cursor to (0,0)
    void clear();

    /// Set the display's master current. Higher current values produce a
    /// brighter, more vivid image but may increase image burn-in and audible
    /// noise from the OLED driver. The default is .75.
    void setCurrent(float current);

    /// Set the per channel contrast values, which affect image brightness and
    /// color balance. The default is (.93, 0.555, 1.0).
    void setContrast(float r, float g, float b);

    /// Set the current line color
    void setLineColor(const Color &c);

    /// Set the current line color
    void setLineColor(uint8_t r, uint8_t g, uint8_t b) {
        setLineColor(Color(r,g,b));
    }
    /// Set the current fill color
    void setFillColor(const Color &c);

    /// Set the current fill color
    void setFillColor(uint8_t r, uint8_t g, uint8_t b) {
        setFillColor(Color(r,g,b));
    }

    /// Set the current text color
    void setTextColor(const Color &textColor);

    /// Set the current text color
    void setTextColor(uint8_t r, uint8_t g, uint8_t b) {
        setTextColor(Color(r,g,b));
    }

    /// Set the text size. This is a multiplier of the base text size,
    /// which is 8px high.
    void setTextSize(uint8_t size);

    /// Fill the screen.
    void fillScreen(Color color);

    /// Fill the screen.
    void fillScreen(uint8_t r, uint8_t g, uint8_t b) {
        fillScreen(Color(r,g,b));
    }

    /// Move the cursor
    void setCursor(int x, int y);

    /// Draw a line segment from (x0,y0) to (x1,y1)
    void drawLine(int x0, int y0, int x1, int y1);

    /// Draw a rectangle with the upper left corner at (x,y) and the
    /// specified width, height, and corner radius.
    void drawRect(int x, int y, int w, int h, int radius=0);

    /// Draw a circle centered at (x,y) with the specified radius
    void drawCircle(int x, int y, int r);

    /// Draw a triangle
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

    /// Write a single charachter c.
    /// Note that the Print methods (print(...), println(...), etc.) are also
    /// available.
    size_t write(uint8_t c);

    /// Display the results of all previous drawing commands.
    /// Note that after calling refresh, the next drawing operation will
    /// block until the frame has been drawn.
    void refresh(bool flip=false);

    /// Return whether all previous drawing operations have been completed.
    bool isComplete();

    /// Return whether the queue of drawing operations is empty. If the display
    /// is still refreshing, it may be empty but not complete.
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

    /// A callback function
    typedef void (EventCallback)(DisplayModulo &module, int button);

    /// Set the function that should be called when a button is pressed.
    void setButtonPressCallback(EventCallback *handler);

    /// Set the function that should be called when a button is released.
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
