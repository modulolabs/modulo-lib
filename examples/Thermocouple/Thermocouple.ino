#include "Modulo.h"
#include "Wire.h"

MiniDisplayModule display;
ThermocoupleModule thermocouple;
KnobModule knob;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

int getSetTemp() {
    return knob.getPosition()*5 + 325;
}

void loop() {
  // put your main code here, to run repeatedly:

  int temp = thermocouple.getTemperatureF();
  int setTemp = getSetTemp();
  
  display.setCursor(10,10);
  display.fillScreen(0);
  display.setTextSize(1);
  display.println("Current     Target\n");
  display.setTextSize(2);
  
  display.setCursor(0,40);
  display.print((int)temp);
  display.print("\xF7" "F");

  display.setCursor(display.width()/2,40);
  display.print(setTemp);
  display.print("\xF7" "F");

  if (abs(temp-setTemp) < 10) {
      knob.setColor(0,255,0);
  } else if (temp > getSetTemp()) {
     knob.setColor(255,0,0);
  } else {
     knob.setColor(0,0,255);
  }

  display.refresh();
    
}

