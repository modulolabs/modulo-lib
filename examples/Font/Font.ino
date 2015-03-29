#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;

void setup() {

}

const int numPages = 11;
int page = 0;
bool nextButtonWasPressed = false;
bool prevButtonWasPressed = false;

void loop() {
    display.clear();
    display.setCursor(0,0);

    bool nextButtonIsPressed = display.getButton(2);
    bool prevButtonIsPressed = display.getButton(0);

    if (nextButtonIsPressed and !nextButtonWasPressed) {
        page = (page + 1) % numPages;
    }

    if (prevButtonIsPressed and !prevButtonWasPressed) {
        page = (page - 1 + numPages) % numPages;
    }

    nextButtonWasPressed = nextButtonIsPressed;
    prevButtonWasPressed = prevButtonIsPressed;

    for (int i=0; i < 8; i++) {
        for (int column=0; column < 3; column++) {

            int c = page*24 + column*8 + i;

            if (c < 256) {
                display.setCursor(column*40, i*8);
                display.print(c);
                display.print(": ");
                display.print(char(c));
            }
        }
        display.println();
    }
    display.refresh();

}