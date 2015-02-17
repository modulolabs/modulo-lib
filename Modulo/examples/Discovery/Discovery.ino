#include "Modulo.h"
#include "Wire.h"
#include <inttypes.h>


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("");
  Serial.println("");
  
  ModuloGlobalReset();
  
  //uint8_t address = 1;
  uint8_t nextAddress = 100;
  uint16_t deviceID = ModuloGetNextDeviceID(0);
  char buffer[32];
  char deviceType[32];
  while (deviceID != 0xFFFF) {
      Serial.print("Found device: ");
      Serial.println(deviceID, HEX);
      
      ModuloSetAddress(deviceID, nextAddress++);
      
      
      
      Serial.print("    Device Type: ");
      if(ModuloGetDeviceType(deviceID, deviceType, 31)) {
          Serial.println(deviceType);
      } else {
          deviceType[0] = 0;
      }
 
      Serial.print("    Company Name: ");
      if(ModuloGetCompanyName(deviceID, buffer, 31)) {
          Serial.println(buffer);
      }
      
      Serial.print("    Product Name: ");
      if(ModuloGetProductName(deviceID, buffer, 31)) {
          Serial.println(buffer);
      }
      
      Serial.print("    Doc URL: ");
      if(ModuloGetDocURL(deviceID, buffer, 31)) {
          Serial.println(buffer);
      }
      
      uint8_t address = ModuloGetAddress(deviceID);
      Serial.print("    Address: ");
      Serial.println(address);
      
      if (strcmp(deviceType,"co.modulo.Thermocouple") == 0) {
          ModThermocouple thermocouple(address);
          Serial.print("    Temperature: ");
          float temp = thermocouple.getTemperature();
          if (temp == ModThermocouple::InvalidTemperature) {
              Serial.println("Invalid");
          } else {
              Serial.println(thermocouple.getTemperature());
          }
      }
      
      if (strcmp(deviceType,"co.modulo.DPad") == 0) {
          ModDPad dPad(address);
          
          dPad.sync();
          Serial.print("   State: ");
          Serial.println(dPad.getStates(), BIN);
          Serial.print("Presseds: ");
          Serial.println(dPad.getPresses(), BIN);
          Serial.print("Releases: ");
          Serial.println(dPad.getReleases(), BIN);
      }
      
      if (strcmp(deviceType,"co.modulo.rtc") == 0) {
          ModTime clock(address);
        
          ModTime::Time t = clock.getTime();
          
          const char *days[7]  = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
          const char *day = days[t.weekdays];
          Serial.print("    ");
          Serial.print(day);
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
          Serial.print(t.seconds);
          Serial.print(" ");
          if (t.clockSet) {
              Serial.print(" <set> ");
          } else {
              Serial.print(" <not set> ");
          }
          if (t.battLow) {
              Serial.println(" <battery low> ");
          } else {
              Serial.println(" <battery ok> ");
          }
          
      }
      
      deviceID = ModuloGetNextDeviceID(deviceID+1);
  } 
  Serial.println("No more devices");

  delay(500);
}

