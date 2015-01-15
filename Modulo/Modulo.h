#ifndef MODULO_H
#define MODULO_H

#include "IOModule.h"
#include "MiniDisplayModule.h"
#include "KnobModule.h"
#include "ClockModule.h"
#include "DPadModule.h"
#include "ThermocoupleModule.h"

void ModuloSetHighBitRate();

void ModuloGlobalReset();

uint16_t ModuloGetNextDeviceID(uint16_t lastDeviceID);

bool ModuloSetAddress(uint16_t deviceID, uint8_t address);

uint8_t ModuloGetAddress(uint16_t deviceID);

bool ModuloGetDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen);

bool ModuloGetCompanyName(uint16_t deviceID, char *deviceType, uint8_t maxLen);

bool ModuloGetProductName(uint16_t deviceID, char *deviceType, uint8_t maxLen);

bool ModuloGetDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen);

bool moduloTransfer(
    uint8_t address, uint8_t command, uint8_t *sendData, uint8_t sendLen,
    uint8_t *receiveData, uint8_t receiveLen, uint8_t retries=3);

#endif

