#include "MotorDriver.h"
#include "Wire.h"

enum MotorDriverFunctions {
  MotorDriverEnA,
  MotorDriverFreqA,
  MotorDriverOutA1,
  MotorDriverOutA2,
  MotorDriverEnB,
  MotorDriverFreqB,
  MotorDriverOutB1,
  MotorDriverOutB2,
  MotorDriverStatusLED = 207
};

MotorDriver::MotorDriver(uint16_t address) : _address(address)
{

}

void MotorDriver::SetEnableA(bool enable) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverEnA);
    Wire.write(enable);
    Wire.endTransmission();
}


void MotorDriver::SetEnableB(bool enable) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverEnB);
    Wire.write(enable);
    Wire.endTransmission();
}
    
void MotorDriver::SetFrequencyA(uint16_t frequency) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverFreqA);
    Wire.write(frequency & 0xFF);
    Wire.write(frequency >> 8);
    Wire.endTransmission();
}

void MotorDriver::SetFrequencyB(uint16_t frequency) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverFreqB);
    Wire.write(frequency & 0xFF);
    Wire.write(frequency >> 8);
    Wire.endTransmission();
}
    
void MotorDriver::SetOutputA1(uint16_t value) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverOutA1);
    Wire.write(value & 0xFF);
    Wire.write(value >> 8);
    Wire.endTransmission();
}

void MotorDriver::SetOutputA2(uint16_t value) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverOutA2);
    Wire.write(value & 0xFF);
    Wire.write(value >> 8);
    Wire.endTransmission();
}

void MotorDriver::SetOutputB1(uint16_t value) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverOutB1);
    Wire.write(value & 0xFF);
    Wire.write(value >> 8);
    Wire.endTransmission();
}

void MotorDriver::SetOutputB2(uint16_t value) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverOutB2);
    Wire.write(value & 0xFF);
    Wire.write(value >> 8);
    Wire.endTransmission();
}

void MotorDriver::SetStatus(uint8_t status) {
    Wire.beginTransmission(_address);
    Wire.write(MotorDriverStatusLED);
    Wire.write(status);
    Wire.endTransmission();
}
    
    
