#include <Wire.h>
#include "Modulo.h"


void DisplayTime(ModuloMiniDisplay &display, ModClock::Time time) {
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

void DisplayDate(ModuloMiniDisplay &display, uint16_t year, uint8_t month, uint8_t day) {
    display.setTextSize(1);

    month = min(max(month,1), 12);
    
    char *monthNames[] = {"January", "February", "March", "April", "May", "June",
                          "July", "August", "September", "October", "November", "December"};

    char *monthName = monthNames[month-1];

    int len = strlen(monthName) + (day >= 10) + 8;
    int charWidth = 6;
    int xPos = (ModuloMiniDisplay::WIDTH-len*charWidth)/2;

    display.setCursor(xPos, 7);
    display.print(monthName);
    display.print(' ');
    display.print(day);
    display.print(", ");
    display.print(year);
}

void DisplaySeconds(ModuloMiniDisplay &display, double time) {
    int32_t ms = time;
    int i = (ModuloMiniDisplay::WIDTH*2 + ModuloMiniDisplay::HEIGHT*2)*(ms % 60000)/60000;
    
    for (int x = ModuloMiniDisplay::WIDTH/2; x < ModuloMiniDisplay::WIDTH; x++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, 0, WHITE);
    }
    for (int y = 0; y < ModuloMiniDisplay::HEIGHT; y++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(ModuloMiniDisplay::WIDTH-1, y, WHITE);
    }
    for (int x = ModuloMiniDisplay::WIDTH-1; x >= 0; x--) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, ModuloMiniDisplay::HEIGHT-1, WHITE);
    }
    for (int y = ModuloMiniDisplay::HEIGHT-1; y >= 0; y--) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(0, y, WHITE);
    }
    
    for (int x = 0; x < ModuloMiniDisplay::WIDTH/2; x++) {
        if (--i < 0) {
            return;
        }
        display.drawPixel(x, 0, WHITE);
    }

}

void DisplayTemperature(ModuloMiniDisplay &display, float temp) {
    int xPos = (ModuloMiniDisplay::WIDTH - 19*6)/2;
    int tempF = temp*9/5.0 + 32;
    display.setCursor(xPos,50);
    display.print("Temperature: ");
    display.print(tempF);
    display.print("\xF7" "F");
}

void DisplayFont(ModuloMiniDisplay &display) {
    display.setCursor(0,0);
    for(int i=150; i < 256; i++) {
        display.print((char)i);
    }
    display.print((char)(256-15+6));
}

ModuloMiniDisplay display;
ModClock clockModule;

void PrintDateAndTime(const ModClock::Time &t)
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
    
    ModClock::Time t;
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
}



void loop() {
    uint32_t time = millis();

    ModClock::Time t = clockModule.getTime();
    
    display.fillScreen(0);
    DisplayTime(display, t);
    DisplayDate(display, t.years, t.months, t.days);
    DisplaySeconds(display, t.seconds*1000.0);
    DisplayTemperature(display, clockModule.getTemperature());
    
    display.display();
}
