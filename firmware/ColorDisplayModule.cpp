#include "ColorDisplayModule.h"
#include "Modulo.h"

#ifdef SPARK
#include "spark_wiring.h"
#else
#include "Arduino.h"
#endif

#define FUNCTION_APPEND_OP 0
#define FUNCTION_IS_COMPLETE 1

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
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::setFillColor(const Color &color) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetFillColor, color.r, color.g, color.b, color.a};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::setTextColor(const Color &color) {
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetTextColor, color.r, color.g, color.b, color.a};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::setCursor(int x, int y)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpSetCursor, x, y};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 3, 0, 0);   
}

void ColorDisplayModule::refresh()
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpRefresh};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 1, 0, 0);

    _isRefreshing = true;
}


void ColorDisplayModule::fillScreen(Color color)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpFillScreen, color.r, color.g, color.b, color.a};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}


void ColorDisplayModule::drawLine(int x0, int y0, int x1, int y1)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawLine, x0, y0, x1, y1};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void ColorDisplayModule::drawRect(int x, int y, int w, int h, int radius)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawRect, x, y, w, h, radius};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 6, 0, 0);
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
        moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, i, 0, 0);
        i = 0;
    }

       
}

size_t ColorDisplayModule::write(uint8_t c) {
    uint8_t sendData[] = {OpDrawString,c,0};
    moduloTransfer(getAddress(), FUNCTION_APPEND_OP, sendData, 3, 0, 0);
    return 1;
}

bool ColorDisplayModule::isComplete() {
    uint8_t complete = 0;
    moduloTransfer(getAddress(), FUNCTION_IS_COMPLETE, 0, 0, &complete, 1);
    return complete;
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

