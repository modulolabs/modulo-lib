#include <Wire.h>
#include "Modulo.h"
#include "Arduino.h"

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
  if (Wire.endTransmission() != 0) {
      Serial.println("Write failed");
      return false;
  }

  //Wire.endTransmission(false /*stop*/);

  // Attiny48/88 seems to require a longer delay between a stop and start
  // The extra delay isn't needed when using repeated starts, however.
    delayMicroseconds(1000);

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
    
    // XXX: Hack to detect the end of variable length strings.
    if (i > 0 and receiveLen == 31 and
        receiveData[i-1] == 0 and receiveData[i] == crc) {
        return true;
    }
    crc = _crc8_ccitt_update(crc, receiveData[i]);
  }
  
  // Receive the CRC.
  uint8_t receivedCRC = Wire.read();

  if(crc != receivedCRC) {
      Serial.println("crc failed");
  }
  
  // Check the CRC.
  return (crc == receivedCRC);
}

bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries)
{
    for (int i=0; i < retries; i++) {
        if (_moduloTransfer(address,command,sendData,sendLen,receiveData,receiveLen)) {
            return true;
        }
    }
    return false;
}


uint16_t ModBase::GetDeviceID(uint8_t address) {
    uint8_t receivedData[2] = {0,0};
    if (!moduloTransfer(address, 254, 0, 0, receivedData, 2)) {
        return -1;
    }
    return receivedData[1] << 8 | receivedData[0];
}

ModThermocouple::ModThermocouple(uint8_t address) : _address(address) {

}

#define FUNCTION_GET_TEMPERATURE_A 0
#define FUNCTION_GET_TEMPERATURE_B 1

const float ModThermocouple::InvalidTemperature = 6553.5;
  
float ModThermocouple::getTemperature() {
    uint8_t receiveData[2] = {0,0};

    if (!moduloTransfer(_address, FUNCTION_GET_TEMPERATURE_A, 0, 0,
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


///

ModDPad::ModDPad(uint8_t address) : _address(address) {}

bool ModDPad::sync() {
    uint8_t receivedData[3] = {0,0,0};
    if (!moduloTransfer(_address, 0, 0, 0, receivedData, 3)) {
        return false;
    }
    _state = receivedData[0];
    _pressed = receivedData[1];
    _released = receivedData[2];
    return true;
}

bool ModDPad::getState(int button) {
    return _state & (1 << button);
}

uint8_t ModDPad::getStates() {
    return _state;
}

bool ModDPad::getPressed(int button) {
    return _pressed & (1 << button);
}

uint8_t ModDPad::getPresses() {
    return _pressed;
}

bool ModDPad::getReleased(int button) {
    return _released & (1 << button);
}

uint8_t ModDPad::getReleases() {
    return _released;
}

///

#define FUNCTION_KNOB_GET_STATE 0
#define FUNCTION_KNOB_ADD_POSITION_OFFSET 1
#define FUNCTION_KNOB_SET_COLOR 2

ModKnob::ModKnob(uint8_t address) : _address(address),
                                    _position(0), 
                                    _buttonState(false),
                                    _buttonPressed(false),
                                    _buttonReleased(false)
{
}

bool ModKnob::setColor(float r, float g, float b) {
    uint8_t sendData[3] = {r*255, g*255, b*255};
    return moduloTransfer(_address, FUNCTION_KNOB_SET_COLOR, sendData, 3, 0, 0);
}

bool ModKnob::sync() {
    uint8_t receivedData[5];
    if (!moduloTransfer(_address, FUNCTION_KNOB_GET_STATE, 0, 0, receivedData, 5)) {
        return false;
    }
    
    _position = receivedData[0] | (receivedData[1] << 8);
    _buttonState = receivedData[2];
    _buttonPressed = receivedData[3];
    _buttonReleased = receivedData[4];
}

bool ModKnob::getButtonState() {
    return _buttonState;
}

bool ModKnob::getButtonPressed() {
    return _buttonPressed;
}

bool ModKnob::getButtonReleased() {
    return _buttonReleased;
}

int16_t ModKnob::getPosition() {
    return _position;
}

