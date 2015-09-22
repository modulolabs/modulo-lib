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
    /// ModuloSetup should be called from the sketch setup function.
    static void setup(bool highBitRate = true);

    /// ModuloLoop should be called from the sketch loop function.
    static void loop();

    /// Reset all devices on the bus to their power-up state
    static void globalReset();

    /// Search the bus for the device with the lowest deviceID that is
    /// greater than or equal to the one provided.
    static uint16_t getNextDeviceID(uint16_t lastDeviceID);

    /// Set a device's I2C address.
    static bool setAddress(uint16_t deviceID, uint8_t address);

    /// Get a device's I2C address
    static uint8_t getAddress(uint16_t deviceID);

    /// Get a device's deviceType
    static bool getDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen);

    /// Get a device's manufacturer
    static bool getManufacturer(uint16_t deviceID, char *deviceType, uint8_t maxLen);

    /// Get a device's product name
    static bool getProduct(uint16_t deviceID, char *deviceType, uint8_t maxLen);

    /// Get a device's documentation url
    static bool getDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen);

    /// Set a device's status LED to Off, On, or Blinking.
    static bool setStatus(uint16_t deviceID, ModuloStatus status);

    /// Perform a data transfer on the modulo bus
    static bool transfer(
        uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
        uint8_t *receiveData, uint8_t receiveLen);
};

extern _Modulo Modulo;

#endif
