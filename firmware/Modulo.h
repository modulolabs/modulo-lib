#ifndef MODULO_H
#define MODULO_H

#define MODULO_CUSTOM_WIRE 0

#include "BlankSlateModulo.h"
#include "ControllerModulo.h"
#include "DisplayModulo.h"
#include "JoystickModulo.h"
#include "KnobModulo.h"
#include "MotorDriverModulo.h"
#include "TemperatureProbeModulo.h"
#include "ThermocoupleModulo.h"
#include "IRRemoteModulo.h"

enum ModuloStatus {
    ModuloStatusOff,
    ModuloStatusOn,
    ModuloStatusBlinking
};

class _Modulo {
public:
    /// loop should be called from the sketch loop function.
    static void loop();

    /// setup is called automatically
    static void setup();

    /// Reset all devices on the bus to their power-up state
    static void globalReset();

    /// Search the bus for the device with the lowest deviceID that is
    /// greater than or equal to the one provided.
    static uint16_t getNextDeviceID(uint16_t lastDeviceID);

    /// Set a device's I2C address. The address is not persistant. It will be
    /// cleared when the modulo is reset or powered off.
    static bool setAddress(uint16_t deviceID, uint8_t address);

    /// Get a device's I2C address, if it currently has one.
    static uint8_t getAddress(uint16_t deviceID);

    /// Get a device's deviceType. 
    static bool getDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen);

    /// Set a device's status LED to Off, On, or Blinking.
    static bool setStatus(uint16_t deviceID, ModuloStatus status);

    /// Perform a data transfer on the modulo bus. This is normally only used
    /// internally within the various Modulo classes.
    static bool transfer(
        uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen, bool receiveString = false);
};

extern _Modulo Modulo;

#endif
