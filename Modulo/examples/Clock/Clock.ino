#include <Wire.h>
#include "Modulo.h"


void DisplayTime(MiniDisplayModule &display, ClockModule::Time time) {
    display.setCursor(15, 22);
    display.setTextSize(3);
    
    uint8_t hours = time.hour;
    uint8_t minutes = time.minute;
    uint8_t seconds = time.second;
    bool pm = hours >= 12;
    
    hours = hours % 12;
    
    if (hours == 0) {
        hours = 12;
    }
    if (hours < 10) {
        display.print(' ');
    }

    display.print(hours);
    display.print(":");
    display.print(minutes/10);
    display.print(minutes % 10);
    
    display.setTextSize(1);
    display.print(pm ? "pm" : "am");
}

void DisplayDate(MiniDisplayModule &display, uint16_t year, uint8_t month, uint8_t day) {
    display.setTextSize(1);

    month = min(max(month,1), 12);
    
    char *monthNames[] = {"January", "February", "March", "April", "May", "June",
                          "July", "August", "September", "October", "November", "December"};

    char *monthName = monthNames[month-1];

    int len = strlen(monthName) + (day >= 10) + 8;
    int charWidth = 6;
    int xPos = (MiniDisplayModule::WIDTH-len*charWidth)/2;

    display.setCursor(xPos, 7);
    display.print(monthName);
    display.print(' ');
    display.print(day);
    display.print(", ");
    display.print(year);
}

void DisplaySeconds(MiniDisplayModule &display, double time) {
    int32_t ms = time;
    int i = (MiniDisplayModule::WIDTH*2 + MiniDisplayModule::HEIGHT*2)*(ms % 60000)/60000;
    
    for (int x = MiniDisplayModule::WIDTH/2; x < MiniDisplayModule::WIDTH; x++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, 0, WHITE);
    }
    for (int y = 0; y < MiniDisplayModule::HEIGHT; y++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(MiniDisplayModule::WIDTH-1, y, WHITE);
    }
    for (int x = MiniDisplayModule::WIDTH-1; x >= 0; x--) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, MiniDisplayModule::HEIGHT-1, WHITE);
    }
    for (int y = MiniDisplayModule::HEIGHT-1; y >= 0; y--) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(0, y, WHITE);
    }
    
    for (int x = 0; x < MiniDisplayModule::WIDTH/2; x++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, 0, WHITE);
    }

}

void DisplayTemperature(MiniDisplayModule &display, float temp) {
    int xPos = (MiniDisplayModule::WIDTH - 19*6)/2;
    display.setCursor(xPos,50);
    display.print("Temperature: ");
    display.print(int(temp));
    display.print("\xF7" "F");
}

void DisplayFont(MiniDisplayModule &display) {
    display.setCursor(0,0);
    for(int i=150; i < 256; i++) {
        display.print((char)i);
    }
    display.print((char)(256-15+6));
}

MiniDisplayModule display;
ClockModule clockModule;
KnobModule knob;

void PrintDateAndTime(const ClockModule::Time &t)
{
    Serial.print(t.month);
    Serial.print("/");
    Serial.print(t.day);
    Serial.print("/");
    Serial.print(t.year);
    Serial.print(" ");
    Serial.print(t.hour);
    Serial.print(":");
    Serial.print(t.minute);
    Serial.print(":");
    Serial.print(t.second);
}

bool GetTimeAndDate() {
    if (!Serial.available()) {
        return false;
    }
    
    Serial.setTimeout(0);
    
    ClockModule::Time t;
    t.month = Serial.parseInt();
    t.day = Serial.parseInt();
    t.year = Serial.parseInt();
    t.hour = Serial.parseInt();
    t.minute = Serial.parseInt();
    t.second = Serial.parseInt();

      
    while (Serial.read() != -1)
        ;
    
    if (t.year == 0) {
        Serial.println();
        Serial.print("The temperature is: ");
        Serial.print(clockModule.getTemperatureC());
        Serial.println("C");
        Serial.print("The current time is: ");
        t = clockModule.getTime();
        PrintDateAndTime(t);
        Serial.println();

        Serial.println("To set the date and time, enter it in the same format");
    } else {
        Serial.print("Setting date and time to: ");
        PrintDateAndTime(t);
        Serial.println();
        clockModule.setTime(t);
    }
    
}


void setup() {
    Serial.begin(9600);
    ModuloSetup();
}

bool timerMode = false;

bool button1Pressed = false;
int knobStartPosition = 0;

bool knobWasPressed = false;
long timerStartTime = 0;
int timerDuration = 0;
bool timerRunning = false;

void loop() {
    ModuloLoop();

    if (display.getButton(1)) {
        if (!button1Pressed) {
            timerMode = !timerMode;
            timerRunning = false;
        }
        button1Pressed = true;
    } else {
        button1Pressed = false;
    }


    if (!timerMode) {
        ClockModule::Time t = clockModule.getTime();
    
        knob.setColor(0,0,0);
        display.fillScreen(0);
        DisplayTime(display, t);
        DisplayDate(display, t.year, t.month, t.day);
        DisplaySeconds(display, t.second*1000.0);
        DisplayTemperature(display, clockModule.getTemperatureF());
    } else {
        int knobPosition = knob.getPosition();
        if (knobPosition < knobStartPosition) {
            knobStartPosition = knobPosition;
        }

        bool knobIsPressed = knob.getButton();
        if (knobIsPressed and !knobWasPressed) {
            timerRunning = !timerRunning;
            timerStartTime = millis();
        }
        knobWasPressed = knobIsPressed;


        int displayTime = 0;
        if (timerRunning) {
            displayTime = timerDuration - (millis()-timerStartTime)/1000;
            displayTime = max(0, displayTime);
        } else {
            timerDuration = knobPosition - knobStartPosition;
            displayTime = timerDuration;
        }

        display.fillScreen(0);
        display.setCursor(0,0);
        display.setTextSize(2);
        if (timerRunning) {
            display.printlnCentered("Running");
        } else {
            display.printlnCentered("Stopped");
        }

        char timeString[64];
        sprintf(timeString, "%d:%02d", displayTime/60, displayTime % 60);

        display.setTextSize(4);
        display.setCursor(0, 30);
        display.printlnCentered(timeString);
        display.setTextSize(1);

        if (timerRunning and displayTime == 0) {
            knob.setColor(millis() % 500 > 250, 0, 0);
        } else {
            knob.setColor(0,0,0);
        }

    }

    //GetTimeAndDate();
    
    display.refresh();
}
