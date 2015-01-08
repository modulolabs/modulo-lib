#ifndef MODULO_BROADCAST_H
#define MODULO_BROADCAST_H

#include <inttypes.h>

void ModuloGlobalReset();
uint16_t ModuloGetNextDeviceID(uint16_t lastDeviceID);
bool ModuloSetAddress(uint16_t deviceID, uint8_t address);
uint8_t ModuloGetAddress(uint16_t deviceID);
bool ModuloGetDeviceType(uint16_t deviceID, char *deviceType, uint8_t maxLen);
bool ModuloGetCompanyName(uint16_t deviceID, char *deviceType, uint8_t maxLen);
bool ModuloGetProductName(uint16_t deviceID, char *deviceType, uint8_t maxLen);
bool ModuloGetDocURL(uint16_t deviceID, char *deviceType, uint8_t maxLen);

#endif
