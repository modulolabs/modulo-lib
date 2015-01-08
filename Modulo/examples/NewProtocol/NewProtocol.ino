// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#include "Modulo.h"

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
}


ModIO io(5);
ModThermocouple thermocouple(9);

void loop()
{
/*
  uint8_t buffer[32];
  buffer[0] = 0;
  
  if (moduloTransfer(5, 0, NULL, 0, buffer, 3)) {
    for (int i=0; i < 3; i++) {
      Serial.print(buffer[i]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  */
    
    //io.setPullup(2);
  //  io.setPWMOutput(0, pow(io.getAnalogInput(3), 2.2));
    //Serial.print(thermocouple.getTemperatureA());
    //Serial.print(" ");
    //Serial.println(thermocouple.getTemperatureB());
    ModTime time(6);
    ModTime::Time t = time.getTime();
    Serial.print("Set: ");
    Serial.print(t.clockSet);
    Serial.print(" Battery Low: ");
    Serial.print(t.battLow);
    Serial.print(" ");
    Serial.print(t.weekdays);
    Serial.print(" ");
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
    
#if 0
    uint8_t value = 0;
    if (moduloTransfer(6 /*address*/, 0 /*command*/, 0 /*sendData*/, 0 /*sendLen*/, &value /*receiveData*/, 1 /*receiveLen*/)) {
      Serial.print("Value: ");
      Serial.println(value);
    } else {
      Serial.println("error");                  
    }
#endif
    
    delay(500);
}
