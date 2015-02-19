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
    DisplayDate(display, t.year, t.month, t.day);
    DisplaySeconds(display, t.second*1000.0);
    DisplayTemperature(display, clockModule.getTemperatureF());

    GetTimeAndDate();
    
    display.display();
}
