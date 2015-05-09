/*
  twi.h - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MODULO_TWI_H
#define MODULO_TWI_J

  #include <inttypes.h>

  //#define ATMEGA8

  #ifndef MODULO_TWI_FREQ
  #define MODULO_TWI_FREQ 100000L
  #endif

  #ifndef MODULO_TWI_BUFFER_LENGTH
  #define MODULO_TWI_BUFFER_LENGTH 32
  #endif

  #define MODULO_TWI_READY 0
  #define MODULO_TWI_MRX   1
  #define MODULO_TWI_MTX   2
  #define MODULO_TWI_SRX   3
  #define MODULO_TWI_STX   4
  
  void modulo_twi_init();
  void modulo_twi_setAddress(uint8_t);
  uint8_t modulo_twi_readFrom(uint8_t, uint8_t, uint8_t);
  uint8_t modulo_twi_read();

  uint8_t modulo_twi_beginWrite(uint8_t address);
  bool modulo_twi_write(uint8_t data);
  uint8_t modulo_twi_endWrite(uint8_t wait, uint8_t sendStop);

  //uint8_t modulo_twi_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t);
  //uint8_t modulo_twi_transmit(const uint8_t*, uint8_t);
  

  void modulo_twi_attachSlaveRxEvent( void (*)(uint8_t*, int) );
  void modulo_twi_attachSlaveTxEvent( void (*)(void) );
  //void modulo_twi_reply(uint8_t);
  //void modulo_twi_stop(void);
  //void modulo_twi_releaseBus(void);

#endif

