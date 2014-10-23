#include "Modulo.h"

bool ModuloReadString(int address, uint8_t registerID, char *buffer) {

  
  Wire.beginTransmission(address);
  Wire.write(registerID);
  Wire.endTransmission();
  
  // Can we change this so it operations in a singler read? Currently collision detection
  // doesn't work correctly, but if we never need to read strings from the default address
  // then maybe that's okay.
  uint8_t numBytes = Wire.requestFrom(address, 1, false /*stop*/);
  if (numBytes != 1) {
    return false;
  }
  int len = Wire.read();
  
  numBytes = Wire.requestFrom(address, len, true /*stop*/);
  if (numBytes == 0) {
    return false;
  }
  
  buffer[numBytes] = 0;

  for (unsigned int i = 0; i < numBytes; i++) {
    buffer[i] = Wire.read();
  }
  
  return true;
}



