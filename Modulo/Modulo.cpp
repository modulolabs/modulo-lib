#include <Wire.h>
#include "Modulo.h"

uint8_t
_crc8_ccitt_update (uint8_t inCrc, uint8_t inData)
{
  uint8_t i;
  uint8_t data;
  data = inCrc ^ inData;
  for ( i = 0; i < 8; i++ ) {
    if (( data & 0x80 ) != 0 ) {
      data <<= 1;
      data ^= 0x07;
    } else {
      data <<= 1;
    }
  }
  return data;
}


bool _moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen)
{
  // Star the transmit CRC with the address in the upper 7 bits
  uint8_t crc =  _crc8_ccitt_update(0, address << 1);
  
  Wire.beginTransmission(address);

  // Send the command and length
  Wire.write(command);
  Wire.write(sendLen);
  crc = _crc8_ccitt_update(crc, command);
  crc = _crc8_ccitt_update(crc, sendLen);
  
  // Send the data
  for (int i=0; i < sendLen; i++) {
    Wire.write(sendData[i]);
    crc = _crc8_ccitt_update(crc, sendData[i]);
  }

  // Send the CRC and end the transmission
  Wire.write(crc);
  Wire.endTransmission();

  if (receiveLen == 0) {
    return true;
  }
  
  // Request receiveLen data bytes plus 1 CRC byte.
  if (Wire.requestFrom((int)address, (int)receiveLen+1) != receiveLen+1) {
    return false;
  }
  
  // Start the CRC with the I2C address byte (address in upper 7, 1 in lsb)
  crc = _crc8_ccitt_update(0, address << 1 | 1);
  
  // Receive the data
  for (int i=0; i < 32 and i < receiveLen; i++) {
    receiveData[i] = Wire.read();
    crc = _crc8_ccitt_update(crc, receiveData[i]);
  }
  
  // Receive the CRC.
  uint8_t receivedCRC = Wire.read();

  // Check the CRC.
  return (crc == receivedCRC);
}

bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{
    for (int i=0; i < 3; i++) {
        if (_moduloTransfer(address,command,sendData,sendLen,receiveData,receiveLen)) {
            return true;
        }
    }
    return false;
}


ModThermocouple::ModThermocouple(uint8_t address) : _address(address) {

}

#define FUNCTION_GET_TEMPERATURE_A 0
#define FUNCTION_GET_TEMPERATURE_B 1
  
float ModThermocouple::getTemperatureA() {
    uint8_t receiveData[2] = {0,0};

    if (!moduloTransfer(_address, FUNCTION_GET_TEMPERATURE_A, 0, 0,
                        receiveData, 2)) {
        // Handle error?
    }
    uint16_t value = receiveData[0] | (receiveData[1] << 8);
    return value/10.0;
}

float ModThermocouple::getTemperatureB() {
    uint8_t receiveData[2] = {0,0};

    if (!moduloTransfer(_address, FUNCTION_GET_TEMPERATURE_B, 0, 0,
                        receiveData, 2)) {
        // Handle error?
    }
    uint16_t value = receiveData[0] | (receiveData[1] << 8);
    return value/10.0;
}
  
#define FUNCTION_GET_TIME 0
#define FUNCTION_SET_TIME 1
#define FUNCTION_GET_TEMPERATURE 2

ModTime::ModTime(uint8_t address) : _address(address) {
}

ModTime::Time ModTime::getTime() {
    uint8_t receivedData[9];
    Time t;
    
    if (moduloTransfer(_address, FUNCTION_GET_TIME, 0, 0,
                        receivedData, 9)) {
        t.seconds = receivedData[0];
        t.minutes = receivedData[1];
        t.hours = receivedData[2];
        t.days = receivedData[3];
        t.weekdays = receivedData[4];
        t.months = receivedData[5];
        t.years = receivedData[6];
        t.clockSet = receivedData[7];
        t.battLow = receivedData[8];
    }
    // Handle error?

    return t;
}

void ModTime::setTime(const Time &t) {

}
  
float ModTime::getTemperature() {

}

