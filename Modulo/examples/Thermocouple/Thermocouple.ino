#include "Modulo.h"
#include "Wire.h"

ModuloMiniDisplay display(101);
ModThermocouple thermocouple;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  
  int16_t temp = thermocouple.getTemperature(); 
  Serial.println(temp);

  display.setCursor(10,10);
  display.fillScreen(0);
  display.println("Thermocouple");
 

  display.print("   Device ID: 0x");
  display.println(thermocouple.getDeviceID(), HEX);

  display.print("   Address : ");
  display.println(thermocouple.getAddress());

  display.print("   Temperature: ");
  display.println(temp/10.0);
  display.display();
    
}

