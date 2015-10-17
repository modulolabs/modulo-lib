#include "DisplayModulo.h"
#include "Modulo.h"

#ifdef SPARK
#include "spark_wiring.h"
#else
#include "Arduino.h"
#endif

#define swap(a, b) { int16_t t = a; a = b; b = t; }


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

const DisplayModulo::Color DisplayModulo::Black;
const DisplayModulo::Color DisplayModulo::White(255,255,255);
const DisplayModulo::Color DisplayModulo::Clear(0,0,0,0);


DisplayModulo::DisplayModulo() :
    BaseModulo("co.modulo.colordisplay"), _currentOp(-1), _opBufferLen(0)
{
}

DisplayModulo::DisplayModulo(uint16_t deviceID) :
    BaseModulo("co.modulo.colordisplay", deviceID), _currentOp(-1), _opBufferLen(0)
{
}

DisplayModulo::~DisplayModulo() {

}

void DisplayModulo::_setCurrentOp(uint8_t opCode) {
    if (_currentOp == opCode) {
        return;
    }

    if (_currentOp != -1) {
        _endPreviousOp();
    }

    _currentOp = opCode;
    _opBufferLen = 1;
    _opBuffer[0] = opCode;
}

void DisplayModulo::_appendToOp(uint8_t data) {
    if (_opBufferLen == OP_BUFFER_SIZE) {
        _opBuffer[_opBufferLen++] = 0;
        return;
    }

    _opBuffer[_opBufferLen++] = data;
}

void DisplayModulo::_endPreviousOp() {
    if (_currentOp == OpDrawString) {
        _appendToOp(0);
    }

    if (_opBufferLen != 0) {
        _transfer(FUNCTION_APPEND_OP, _opBuffer, _opBufferLen, 0, 0);
    }
    _opBufferLen = 0;
    _currentOp = -1;
}


void DisplayModulo::clear() {
    fillScreen(Black);
    setCursor(0,0);
}
/*
uint16_t DisplayModulo::Color(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t c;
    c = r >> 3;
    c <<= 6;
    c |= g >> 2;
    c <<= 5;
    c |= b >> 3;

    return c;
}
*/
void DisplayModulo::setLineColor(const Color &color) {
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpSetLineColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void DisplayModulo::setFillColor(const Color &color) {
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpSetFillColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void DisplayModulo::setTextColor(const Color &color) {
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpSetTextColor, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}


void DisplayModulo::setTextSize(uint8_t size) {
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpSetTextSize, size};
    _transfer(FUNCTION_APPEND_OP, sendData, 2, 0, 0);
}

void DisplayModulo::setCursor(int x, int y)
{
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpSetCursor, x, y};
    _transfer(FUNCTION_APPEND_OP, sendData, 3, 0, 0);
}

void DisplayModulo::refresh()
{
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpRefresh};
    _transfer(FUNCTION_APPEND_OP, sendData, 1, 0, 0);

    _isRefreshing = true;
}


void DisplayModulo::fillScreen(Color color)
{
    _waitOnRefresh();

    uint8_t sendData[] = {OpFillScreen, color.r, color.g, color.b, color.a};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}


void DisplayModulo::drawLine(int x0, int y0, int x1, int y1)
{
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawLine, x0, y0, x1, y1};
    _transfer(FUNCTION_APPEND_OP, sendData, 5, 0, 0);
}

void DisplayModulo::drawRect(int x, int y, int w, int h, int radius)
{
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawRect, x, y, w, h, radius};
    _transfer(FUNCTION_APPEND_OP, sendData, 6, 0, 0);
}

void DisplayModulo::drawCircle(int x, int y, int radius)
{
    _endPreviousOp();

    _waitOnRefresh();

    uint8_t sendData[] = {OpDrawCircle, x, y, radius};
    _transfer(FUNCTION_APPEND_OP, sendData, 4, 0, 0);
}

void DisplayModulo::drawString(const char *s)
{

    int l = strlen(s);
    for (int i=0; i < l; i++) {
        write(s[i]);
    }
}


size_t DisplayModulo::write(uint8_t c) {
    _waitOnRefresh();


    if (_opBufferLen == OP_BUFFER_SIZE-2) {
        _endPreviousOp();
    }

    _setCurrentOp(OpDrawString);

    _appendToOp(c);

    return 1;
}

bool DisplayModulo::isComplete() {
    uint8_t complete = 0;
    if (_transfer(FUNCTION_IS_COMPLETE, 0, 0, &complete, 1)) {
        return complete;
    }
    return true;
}

void DisplayModulo::_waitOnRefresh()
{
    if (_isRefreshing) {
        _isRefreshing = false;
        while (!isComplete()) {
            delay(5);
        }
    }
}

bool DisplayModulo::getButton(int button) {

    return getButtons() & (1 << button);
}

uint8_t DisplayModulo::getButtons() {
    uint8_t receivedData[1] = {0};
    if (!_transfer(FUNCTION_GET_BUTTONS, 0, 0, receivedData, 1)) {
        return false;
    }

    return receivedData[0];
}

void DisplayModulo::drawSplashScreen() {
    setFillColor(DisplayModulo::Color(80,0,60));
    setLineColor(DisplayModulo::Color(0,0,0,0));
    drawRect(0, 0, width(), height());
    setCursor(0, 40);

    setTextSize(1);
    print("     MODULO");

    setFillColor(DisplayModulo::Color(255,255,255));

    drawLogo(width()/2-18, 10, 35, 26);

}

void DisplayModulo::drawLogo(int x, int y, int width, int height) {

    int lineWidth = width/7;

    drawRect(x, y, width, lineWidth, 1);
    drawRect(x, y, lineWidth, height, 1);
    drawRect(x+width-lineWidth, y, lineWidth, height, 1);

    drawRect(x+lineWidth*2, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    drawRect(x+lineWidth*4, y+lineWidth*2, lineWidth, height-lineWidth*2, 1);
    drawRect(x+lineWidth*2, y+height-lineWidth, lineWidth*3, lineWidth, 1);
}
