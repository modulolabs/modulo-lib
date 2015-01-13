#include "ModuloBase.h"
#include "Broadcast.h"
#include "Wire.h"


uint8_t ModBase::_lastAssignedAddress = 9;

ModBase::ModBase(const char *deviceType) :
    _deviceType(deviceType), _deviceID(0xFFFF), _address(0xFF) {
}

ModBase::ModBase(const char *deviceType, uint16_t deviceID) : 
    _deviceType(deviceType), _deviceID(deviceID), _address(0xFF) {
}

uint16_t ModBase::getDeviceID() {
    return _deviceID;
}

uint8_t ModBase::getAddress() {
    _initAddress();
    return _address;
}

void ModBase::_initAddress() {
    if (_address != 0xFF) {
        return;
    }

    // Ensure that a global reset has been performed
    static bool _moduloInitialized = false;
    if (!_moduloInitialized) {
        ModuloGlobalReset();
        Wire.begin();
        _moduloInitialized = true;
    }
    
    if (_deviceID == 0xFFFF) {
        // Find the first device with the specified type and no assigned address
        uint16_t deviceID = ModuloGetNextDeviceID(0);
        while (deviceID != 0xFFFF) {
            char deviceType[32] = {0};
            ModuloGetDeviceType(deviceID, deviceType, 31);
            if (strcmp(deviceType,_deviceType) == 0) {
                _deviceID = deviceID;
                break;
            }
            
            deviceID = ModuloGetNextDeviceID(deviceID+1);
        }
    }

    if (_deviceID == 0xFFFF) {
        // Couldn't find a device
        return;
    }
    
    _address = ++_lastAssignedAddress;
    ModuloSetAddress(_deviceID, _address);
}



void ModuloSetHighBitRate() {
    TWBR = 12;
    TWSR &= ~(3);
}

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
      return false;
  }

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

