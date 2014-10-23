#include <inttypes.h>
#include <Wire.h>

#define MODULO_COMMAND_COMPANY_NAME 200
#define MODULO_COMMAND_DEVICE_NAME 201
#define MODULO_COMMAND_DEVICE_VERSION 202
#define MODULO_COMMAND_DEVICE_ID 203
#define MODULO_COMMAND_DOC_URL 204
#define MODULO_COMMAND_REGISTER_NAMES 205
#define MODULO_COMMAND_REGISTER_TYPES 206
#define MODULO_COMMAND_STATUS_LED 207
#define MODULO_COMMAND_LOT_NUMBER 208
#define MODULO_COMMAND_SERIAL_NUMBER 209
#define MODULO_INVALID_COMMAND 255
#define MODULO_REGISTER_ASSIGN_ADDRESS 252
#define MODULO_REGISTER_START_ENUMERATION 253
#define MODULO_REGISTER_ENUMERATE_NEXT_DEVICE 254
#define MODULO_INVALID_REGISTER 255

bool ModuloReadString(int address, uint8_t registerID, char *buffer);


template <typename ValueType>
bool ModuloRead(int address, uint8_t function, ValueType *result) {
  Wire.beginTransmission(address);
  Wire.write(function);
  Wire.endTransmission();
  
  uint8_t numBytes = Wire.requestFrom(address, sizeof(ValueType));
  if (numBytes != sizeof(ValueType)) {
    return false;
  }
  for (unsigned int i=0; i < sizeof(ValueType); i++) {
    ((uint8_t*)result)[i] = Wire.read();
  }
  return true;
}

template <typename ValueType>
void ModuloWrite(int address, uint8_t function, const ValueType &value) {
  Wire.beginTransmission(address);
  Wire.write(function);

  for (unsigned int i=0; i < sizeof(ValueType); i++) {
    Wire.write(((uint8_t*)&value)[i]);
  }

  Wire.endTransmission();
}


