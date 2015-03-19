#ifndef MODULO_H
#define MODULO_H

#include "IOModule.h"
#include "MiniDisplayModule.h"
#include "KnobModule.h"
#include "ClockModule.h"
#include "DPadModule.h"
#include "ThermocoupleModule.h"
#include "ControllerModule.h"
#include "ColorDisplayModule.h"

enum ModuloStatus {
    ModuloStatusOff,
    ModuloStatusOn,
    ModuloStatusBlinking
};

/// ModuloSetup should be called from the sketch setup function.
void ModuloSetup(bool highBitRate = true);

/// ModuloLoop should be called from the sketch loop function.
void ModuloLoop();

/// Reset all devices on the bus to their power-up state
void ModuloGlobalReset();

/// Search the bus for the device with the lowest deviceID that is
/// greater than or equal to the one provided.
uint16_t ModuloGetNextDeviceID(uint16_t lastDeviceID);

/// Set a device's I2C address.
bool ModuloSetAddress(uint16_t deviceID, uint8_t address);

/// Get a device's I2C address
uint8_t ModuloGetAddress(uint16_t deviceID);

/// Get a device's deviceType
bool ModuloGetDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen);

/// Get a device's manufacturer
bool ModuloGetManufacturer(uint16_t deviceID, char *deviceType, uint8_t maxLen);

/// Get a device's product name
bool ModuloGetProduct(uint16_t deviceID, char *deviceType, uint8_t maxLen);

/// Get a device's documentation url
bool ModuloGetDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen);

/// Set a device's status LED to Off, On, or Blinking.
bool ModuloSetStatus(uint16_t deviceID, ModuloStatus status);

/// Perform a data transfer on the modulo bus
bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries=3);

#endif

