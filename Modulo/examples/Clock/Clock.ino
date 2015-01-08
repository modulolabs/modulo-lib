#define TWI_FREQ 400000L

#include <Wire.h>
#include "MiniDisplay.h"
#include "MotorDriver.h"
#include "Adafruit_GFX.h"
#include "Modulo.h"

void setup() {
  // put your setup code here, to run once:
    Wire.begin();
    TWBR = ((F_CPU / 800000L) - 16) / 2;
    pinMode(13, OUTPUT);
}



void DisplayTime(MiniDisplay &display, ModTime::Time time) {
    display.setCursor(15, 22);
    display.setTextSize(3);
    
    digitalWrite(13, time.seconds % 2);
    
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

void DisplayDate(MiniDisplay &display, uint16_t year, uint8_t month, uint8_t day) {
    display.setTextSize(1);

    month = min(max(month,1), 12);
    
    char *monthNames[] = {"January", "February", "March", "April", "May", "June",
                        "July", "August", "September", "October", "November", "December"};

    char *monthName = monthNames[month-1];

    int len = strlen(monthName) + (day >= 10) + 8;
    int charWidth = 6;
    int xPos = (DISPLAY_WIDTH-len*charWidth)/2;

    display.setCursor(xPos, 7);
    display.print(monthName);
    display.print(' ');
    display.print(day);
    display.print(", ");
    display.print(year);
}

void DisplaySeconds(MiniDisplay &display, double time) {
    int32_t ms = time;
    int i = (DISPLAY_WIDTH*2 + DISPLAY_HEIGHT*2)*(ms % 60000)/60000;
    
    for (int x = DISPLAY_WIDTH/2; x < DISPLAY_WIDTH; x++) {
       if (--i < 0) {
          return;
       }
       display.drawPixel(x, 0, 0xFFFF);
    }
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      if (--i < 0) {
        return;
      }
      display.drawPixel(DISPLAY_WIDTH-1, y, 0xFFFF);
    }
    for (int x = DISPLAY_WIDTH-1; x >= 0; x--) {
        if (--i < 0) {
          return;
        }
        display.drawPixel(x, DISPLAY_HEIGHT-1, 0xFFFF);
    }
    for (int y = DISPLAY_HEIGHT-1; y >= 0; y--) {
        if (--i < 0) {
          return;
        }
        display.drawPixel(0, y, 0xFFFF);
    }
    
    for (int x = 0; x < DISPLAY_WIDTH/2; x++) {
        if (--i < 0) {
          return;
        }
        display.drawPixel(x, 0, 0xFFFF);
    }

}

void DisplayTemperature(MiniDisplay &display, float temp) {
   int xPos = (DISPLAY_WIDTH - 19*6)/2;
    int tempF = temp*9/5.0 + 32;
   display.setCursor(xPos,50);
   display.print("Temperature: ");
   display.print(tempF);
   display.print("\xF7" "F");
}

void DisplayFont(MiniDisplay &display) {
  display.setCursor(0,0);
  for(int i=150; i < 256; i++) {
    display.print((char)i);
  }
  display.print((char)(256-15+6));
}

MiniDisplay display;
MotorDriver motor(14);    
ModTime timeAndTemp(6);

bool GetTimeAndDate() {
    if (!Serial.available()) {
      return false;
    }
    
    Serial.setTimeout(0);
    
    ModTime::Time t;
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
      Serial.println("enter time in the following format: 12/14/2014 h:m:s");  
    } else {
      
      
      Serial.print("Setting date and time to: ");

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
      Serial.println(t.seconds);
    

      
      timeAndTemp.setTime(t);
    }
    
}

void loop() {

    uint32_t time = millis();
      

    display.fillScreen(0);
    
    //display.write('E');
    //display.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
    //display.update();
    
    //display.fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 1);

    ModTime::Time t = timeAndTemp.getTime();
    Serial.println(t.seconds);
    
    display.SetStatus((time/1000) % 5 == 0);
    DisplayTime(display, t);
    DisplayDate(display, t.years, t.months, t.days);
    DisplaySeconds(display, t.seconds*1000.0);
    //DisplayTemperature(display, timeAndTemp.getTemperature());
    //DisplayFont(display);
    /*
    float speed = sin(time*2*M_PI/2000.0);
    if (speed >= 0) {
      motor.SetOutputB2(65535*speed);
      motor.SetOutputB1(0);
      motor.SetStatus(true);
    } else {
      motor.SetOutputB1(-65535*speed);
      motor.SetOutputB2(0);
      motor.SetStatus(false);
    }
    */
    
    display.update();
    //digitalWrite(13, false);
    
    GetTimeAndDate();
}
