#include "ColorDisplayModule.h"
#include "Modulo.h"

#ifdef SPARK
#include "spark_wiring.h"
#else
#include "Arduino.h"
#endif

#define FUNCTION_APPEND_OP 0
#define FUNCTION_IS_COMPLETE 1
#define FUNCTION_GET_BUTTONS 2

static const int OpRefresh = 0;
static const int OpFillScreen = 1;
static const int OpDrawLine = 2;
static const int OpSetLineColor = 3;
static const int OpSetFillColor = 4;
static const int OpSetTextColor = 5;
static const int OpDrawRect = 6;
static const int OpDrawCircle = 7;
static const int OpDrawTriangle = 8;
static const int OpDrawString = 9;
static const int OpSetCursor = 10;
static const int OpSetTextSize = 11;

const ColorDisplayModule::Color ColorDisplayModule::Black;
const ColorDisplayModule::Color ColorDisplayModule::White(255,255,255);
const ColorDisplayModule::Color ColorDisplayModule::Clear(0,0,0,0);


ColorDisplayModule::ColorDisplayModule() :
    Module("co.modulo.colordisplay")
{
}

ColorDisplayModule::ColorDisplayModule(uint16_t deviceID) :
    Module("co.modulo.colordisplay", deviceID)
{
}

ColorDisplayModule::~ColorDisplayModule() {

}

void ColorDisplayModule::clear() {
    fillScreen(Black);
    setCursor(0,0);
}
/*
uint16_t ColorDisplayModule::Color(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t c;
    c = r >> 3;
    c <<= 6;
    c |= g >> 2;
    c <<= 5;
    c |= b >> 3;

    return c;
}
*/
void ColorDisplayModule::setLineColor(const Color &color) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetLineColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::setFillColor(const Color &color) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetFillColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::setTextColor(const Color &color) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetTextColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}


void ColorDisplayModule::setTextSize(uint8_t size) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetTextSize, size};
    _transfer(FUNCTION_APPEND_OP, sendData, 2, 0, 0);
}

void ColorDisplayModule::setCursor(int x, int y)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetCursor, x, y};
    _transfer(FUNCTION_APPEND_OP, sendData, 3, 0, 0);   
}

void ColorDisplayModule::refresh()
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpRefresh};
    _transfer(FUNCTION_APPEND_OP, sendData, 1, 0, 0);

    _isRefreshing = true;
}


void ColorDisplayModule::fillScreen(Color color)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpFillScreen, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}


void ColorDisplayModule::drawLine(int x0, int y0, int x1, int y1)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawLine, x0, y0, x1, y1};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::drawRect(int x, int y, int w, int h, int radius)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawRect, x, y, w, h, radius};
    _transfer(FUNCTION_APPEND_OP, sendData, 6, 0, 0);
}

void ColorDisplayModule::drawString(const char *s)
{
    _waitOnRefresh();

    const int maxLen = 31;
    uint8_t sendData[maxLen] = {OpDrawString};
   
    int i = 1;
    while (*s != 0) {
        while (i < maxLen and *s) {
            sendData[i++] = *(s++);
        }
        if (*s == 0) {
            sendData[i++] = 0;
        }
        _transfer(FUNCTION_APPEND_OP, sendData, i, 0, 0);
        i = 0;
    }

       
}

size_t ColorDisplayModule::write(uint8_t c) {
    uint8_t sendData[] = {OpDrawString,c,0};
    _transfer(FUNCTION_APPEND_OP, sendData, 3, 0, 0);
    return 1;
}

bool ColorDisplayModule::isComplete() {
    uint8_t complete = 0;
    if (_transfer(FUNCTION_IS_COMPLETE, 0, 0, &complete, 1)) {
        return complete;
    }
    return true;
}

void ColorDisplayModule::_waitOnRefresh()
{
  
    if (_isRefreshing) {
        _isRefreshing = false;
        while (!isComplete()) {
            delay(5);
        }
    }
}

bool ColorDisplayModule::getButton(int button) {

    return getButtons() & (1 << button);
}

uint8_t ColorDisplayModule::getButtons() {
    uint8_t receivedData[1] = {0};
    if (!_transfer(FUNCTION_GET_BUTTONS, 0, 0, receivedData, 1)) {
        return false;
    }

    return receivedData[0];
}

void ColorDisplayModule::drawSplashScreen() {
    setFillColor(ColorDisplayModule::Color(90,0,50));
    setLineColor(ColorDisplayModule::Color(0,0,0,0));
    drawRect(0, 0, WIDTH, HEIGHT);
    setCursor(0, 40);

    print("     MODULO");

    setFillColor(ColorDisplayModule::Color(255,255,255));

    drawLogo(WIDTH/2-18, 10, 35, 26);
    
}

void ColorDisplayModule::drawLogo(int x, int y, int width, int height) {

    int lineWidth = width/7;

    drawRect(x, y, width, lineWidth, 1);
    drawRect(x, y, lineWidth, height, 1);
    drawRect(x+width-lineWidth, y, lineWidth, height, 1);

    drawRect(x+lineWidth*2, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    drawRect(x+lineWidth*4, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    drawRect(x+lineWidth*2, y+height-lineWidth, lineWidth*3, lineWidth, 1);
}




