#include <Wire.h>
#include "Modulo.h"


void DisplayTime(MiniDisplayModule &display, ClockModule::Time time) {
    display.setCursor(15, 22);
    display.setTextSize(3);
    
    uint8_t hours = time.hours;
    uint8_t minutes = time.minutes;
    uint8_t seconds = time.seconds;
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
    int tempF = temp*9/5.0 + 32;
    display.setCursor(xPos,50);
    display.print("Temperature: ");
    display.print(tempF);
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

void PrintDateAndTime(const ClockModule::Time &t)
{
    Serial.print(t.months);
    Serial.print("/");
    Serial.print(t.days);
    Serial.print("/");
    Serial.print(t.years);
    Serial.print(" ");
    Serial.print(t.hours);
    Serial.print(":");
    Serial.print(t.minutes);
    Serial.print(":");
    Serial.print(t.seconds);
}

bool GetTimeAndDate() {
    if (!Serial.available()) {
        return false;
    }
    
    Serial.setTimeout(0);
    
    ClockModule::Time t;
    t.months = Serial.parseInt();
    t.days = Serial.parseInt();
    t.years = Serial.parseInt();
    t.hours = Serial.parseInt();
    t.minutes = Serial.parseInt();
    t.seconds = Serial.parseInt();

      
    while (Serial.read() != -1)
        ;
    
    if (t.years == 0) {
        Serial.println();
        Serial.print("The temperature is: ");
        Serial.print(clockModule.getTemperature());
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
    // put your setup code here, to run once:
    Wire.begin();
    Serial.begin(9600);
    ModuloSetHighBitRate();
}



void loop() {
    uint32_t time = millis();

    ModuloSetHighBitRate();

    ClockModule::Time t = clockModule.getTime();
    
    display.fillScreen(0);
    DisplayTime(display, t);
    DisplayDate(display, t.years, t.months, t.days);
    DisplaySeconds(display, t.seconds*1000.0);
    DisplayTemperature(display, clockModule.getTemperature());
    
    display.display();
}
