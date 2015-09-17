/*
  twi.c - TWI/I2C library for Wiring & Arduino
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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
*/


#include "Modulo.h"
#if MODULO_CUSTOM_WIRE

#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <compat/twi.h>
#include "Arduino.h" // for digitalWrite

#define MODULO_TWI_DEBUG 0

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "pins_arduino.h"
#include "ModuloTWI.h"

static volatile uint8_t modulo_twi_state;
static volatile uint8_t modulo_twi_slarw;
static volatile uint8_t modulo_twi_sendStop;			// should the transaction end with a stop
static volatile uint8_t modulo_twi_inRepStart;			// in the middle of a repeated start

static void (*modulo_twi_onSlaveTransmit)(void);
static void (*modulo_twi_onSlaveReceive)(uint8_t*, int);

static uint8_t modulo_twi_masterBuffer[MODULO_TWI_BUFFER_LENGTH];
static volatile uint8_t modulo_twi_masterBufferIndex;
static volatile uint8_t modulo_twi_masterBufferLength;
static uint8_t modulo_twi_masterBufferReadIndex;

static uint8_t modulo_twi_txBuffer[MODULO_TWI_BUFFER_LENGTH];
static volatile uint8_t modulo_twi_txBufferIndex;
static volatile uint8_t modulo_twi_txBufferLength;

static uint8_t modulo_twi_rxBuffer[MODULO_TWI_BUFFER_LENGTH];
static volatile uint8_t modulo_twi_rxBufferIndex;

static volatile uint8_t modulo_twi_error;

void modulo_twi_force_reset(const char *msg) {
#if MODULO_TWI_DEBUG
  Serial.print(modulo_twi_error);
  Serial.print(" Forcing TWI Reset: ");
  Serial.println(msg);
#endif
  TWCR = 0;
  modulo_twi_init();
}

/*
 * Function modulo_twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void modulo_twi_init(void)
{
  // initialize state
  modulo_twi_state = MODULO_TWI_READY;
  modulo_twi_sendStop = true;		// default value
  modulo_twi_inRepStart = false;

  // activate internal pullups for twi.
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / MODULO_TWI_FREQ) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */

  // enable twi module, acks, and twi interrupt
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

/*
 * Function modulo_twi_slaveInit
 * Desc     sets slave address and enables interrupt
 * Input    none
 * Output   none
 */
void modulo_twi_setAddress(uint8_t address)
{
  // set twi slave address (skip over TWGCE bit)
  TWAR = address << 1;
}

/*
 * Function modulo_twi_readFrom
 * Desc     attempts to become twi bus master and read a
 *          series of bytes from a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes to read into array
 *          sendStop: Boolean indicating whether to send a stop at the end
 * Output   number of bytes read
 */
uint8_t modulo_twi_readFrom(uint8_t address, uint8_t length, uint8_t sendStop)
{

  // ensure data will fit into buffer
  if(MODULO_TWI_BUFFER_LENGTH < length){
    return 0;
  }

  // wait until twi is ready, become master receiver
  int i =0;
  while(MODULO_TWI_READY != modulo_twi_state){
    if (i++ > 10000) {
      modulo_twi_force_reset("readFrom waiting for ready state");
      return 0;
    }
  }


  modulo_twi_state = MODULO_TWI_MRX;
  modulo_twi_sendStop = sendStop;
  // reset error state (0xFF.. no error occured)
  modulo_twi_error = 0xFF;

  // initialize buffer iteration vars
  modulo_twi_masterBufferIndex = 0;
  modulo_twi_masterBufferLength = length-1;  // This is not intuitive, read on...
  // On receive, the previously configured ACK/NACK setting is transmitted in
  // response to the received byte before the interrupt is signalled.
  // Therefor we must actually set NACK when the _next_ to last byte is
  // received, causing that NACK to be sent in response to receiving the last
  // expected byte of data.

  // build sla+w, slave device address + w bit
  modulo_twi_slarw = TW_READ;
  modulo_twi_slarw |= address << 1;

  if (true == modulo_twi_inRepStart) {
    // if we're in the repeated start state, then we've already sent the start,
    // (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
    // We need to remove ourselves from the repeated start state before we enable interrupts,
    // since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
    // up. Also, don't enable the START interrupt. There may be one pending from the
    // repeated start that we sent outselves, and that would really confuse things.
    modulo_twi_inRepStart = false;			// remember, we're dealing with an ASYNC ISR
    TWDR = modulo_twi_slarw;
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);	// enable INTs, but not START
  }
  else
    // send start condition
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);

  // wait for read operation to complete
  i = 0;
  while(MODULO_TWI_MRX == modulo_twi_state){
    if (i++ > 10000) {
      modulo_twi_force_reset("readFrom waiting for completion");
      return 0;
    }
  }

  if (modulo_twi_masterBufferIndex < length)
    length = modulo_twi_masterBufferIndex;

  // copy twi buffer to data
  //for(i = 0; i < length; ++i){
  //  data[i] = modulo_twi_masterBuffer[i];
  //}
	modulo_twi_masterBufferReadIndex = 0;
  return length;
}

uint8_t modulo_twi_read() {
    if (modulo_twi_masterBufferReadIndex >= modulo_twi_masterBufferIndex) {
      return 0;
    }
    return modulo_twi_masterBuffer[modulo_twi_masterBufferReadIndex++];
}
bool modulo_twi_available() {
    return (modulo_twi_masterBufferReadIndex >= modulo_twi_masterBufferIndex);
}

/*
 * Function modulo_twi_writeTo
 * Desc     attempts to become twi bus master and write a
 *          series of bytes to a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 *          sendStop: boolean indicating whether or not to send a stop at the end
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
uint8_t modulo_twi_beginWrite(uint8_t address)
{
  // wait until twi is ready, become master transmitter
  while(MODULO_TWI_READY != modulo_twi_state){
    if (TW_STATUS == TW_NO_INFO) {
        modulo_twi_force_reset("beginWrite waiting for ready state");
        return 4;
    }
  }

  modulo_twi_state = MODULO_TWI_MTX;

  // reset error state (0xFF.. no error occured)
  modulo_twi_error = 0xFF;

  // initialize buffer iteration vars
  modulo_twi_masterBufferIndex = 0;
  modulo_twi_masterBufferLength = 0;

  // build sla+w, slave device address + w bit
  modulo_twi_slarw = TW_WRITE;
  modulo_twi_slarw |= address << 1;

  return 0;
}

bool modulo_twi_write(uint8_t data) {
  if (modulo_twi_masterBufferLength >= MODULO_TWI_BUFFER_LENGTH) {
    return false;
  }
  modulo_twi_masterBuffer[modulo_twi_masterBufferLength++] = data;
  return true;
}

bool modulo_twi_write(uint8_t* data, uint8_t length) {
  if (modulo_twi_masterBufferLength+length > MODULO_TWI_BUFFER_LENGTH) {
    return true;
  }

  for (int i=0; i < length; i++) {
    modulo_twi_masterBuffer[modulo_twi_masterBufferLength++] = data[i];
  }

  return false;
}


uint8_t modulo_twi_endWrite(uint8_t wait, uint8_t sendStop) {
  modulo_twi_sendStop = sendStop;

  // if we're in a repeated start, then we've already sent the START
  // in the ISR. Don't do it again.
  //
  if (modulo_twi_inRepStart) {
    // if we're in the repeated start state, then we've already sent the start,
    // (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
    // We need to remove ourselves from the repeated start state before we enable interrupts,
    // since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
    // up. Also, don't enable the START interrupt. There may be one pending from the
    // repeated start that we sent outselves, and that would really confuse things.
    modulo_twi_inRepStart = false;			// remember, we're dealing with an ASYNC ISR
    TWDR = modulo_twi_slarw;
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);	// enable INTs, but not START
  } else {
    // send start condition
    TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);	// enable INTs
  }



  // wait for write operation to complete

  int32_t i = 0;
  while(wait && (MODULO_TWI_MTX == modulo_twi_state)){
    i++;

    if (i > 100000) {
      modulo_twi_force_reset("endWrite waiting for completion");
      return 4;
    }

  }

  if (modulo_twi_error == 0xFF)
    return 0;	// success
  else if (modulo_twi_error == TW_MT_SLA_NACK)
    return 2;	// error: address send, nack received
  else if (modulo_twi_error == TW_MT_DATA_NACK)
    return 3;	// error: data send, nack received
  else
    return 4;	// other twi error
}

/*
 * Function modulo_twi_transmit
 * Desc     fills slave tx buffer with data
 *          must be called in slave tx event callback
 * Input    data: pointer to byte array
 *          length: number of bytes in array
 * Output   1 length too long for buffer
 *          2 not slave transmitter
 *          0 ok
 */
#if 0
uint8_t modulo_twi_transmit(const uint8_t* data, uint8_t length)
{
  uint8_t i;

  // ensure data will fit into buffer
  if(MODULO_TWI_BUFFER_LENGTH < length){
    return 1;
  }

  // ensure we are currently a slave transmitter
  if(MODULO_TWI_STX != modulo_twi_state){
    return 2;
  }

  // set length and copy data into tx buffer
  modulo_twi_txBufferLength = length;
  for(i = 0; i < length; ++i){
    modulo_twi_txBuffer[i] = data[i];
  }

  return 0;
}
#endif

/*
 * Function modulo_twi_attachSlaveRxEvent
 * Desc     sets function called before a slave read operation
 * Input    function: callback function to use
 * Output   none
 */
void modulo_twi_attachSlaveRxEvent( void (*function)(uint8_t*, int) )
{
  modulo_twi_onSlaveReceive = function;
}

/*
 * Function modulo_twi_attachSlaveTxEvent
 * Desc     sets function called before a slave write operation
 * Input    function: callback function to use
 * Output   none
 */
void modulo_twi_attachSlaveTxEvent( void (*function)(void) )
{
  modulo_twi_onSlaveTransmit = function;
}

/*
 * Function modulo_twi_reply
 * Desc     sends byte or readys receive line
 * Input    ack: byte indicating to ack or to nack
 * Output   none
 */
static void modulo_twi_reply(uint8_t ack)
{
  // transmit master read ready signal, with or without ack
  if(ack){
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
  }else{
	  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
  }
}

/*
 * Function modulo_twi_stop
 * Desc     relinquishes bus master status
 * Input    none
 * Output   none
 */
static void modulo_twi_stop(void)
{
  // send stop condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

  // wait for stop condition to be exectued on bus
  // TWINT is not set after a stop condition!

  int i=0;
  while(TWCR & _BV(TWSTO)){
    if (i++ > 10000) {
#if MODULO_TWI_DEBUG
      Serial.println("Got stuck in ISR");
#endif
      break;
    }
    continue;
  }

  // update twi state
  modulo_twi_state = MODULO_TWI_READY;
}

/*
 * Function modulo_twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
static void modulo_twi_releaseBus(void)
{
  // release bus
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);

  // update twi state
  modulo_twi_state = MODULO_TWI_READY;
}

ISR(TWI_vect)
{

  switch(TW_STATUS){
    // All Master
    case TW_START:     // sent start condition
    case TW_REP_START: // sent repeated start condition
      // copy device address and r/w bit to output register and ack
      TWDR = modulo_twi_slarw;
      modulo_twi_reply(1);
      break;

    // Master Transmitter
    case TW_MT_SLA_ACK:  // slave receiver acked address
    case TW_MT_DATA_ACK: // slave receiver acked data
      // if there is data to send, send it, otherwise stop
      if(modulo_twi_masterBufferIndex < modulo_twi_masterBufferLength){
        // copy data to output register and ack
        TWDR = modulo_twi_masterBuffer[modulo_twi_masterBufferIndex++];
        modulo_twi_reply(1);
      }else{
	if (modulo_twi_sendStop)
          modulo_twi_stop();
	else {
	  modulo_twi_inRepStart = true;	// we're gonna send the START
	  // don't enable the interrupt. We'll generate the start, but we
	  // avoid handling the interrupt until we're in the next transaction,
	  // at the point where we would normally issue the start.
	  TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
	  modulo_twi_state = MODULO_TWI_READY;
	}
      }
      break;
    case TW_MT_SLA_NACK:  // address sent, nack received
      modulo_twi_error = TW_MT_SLA_NACK;
      modulo_twi_stop();
      break;
    case TW_MT_DATA_NACK: // data sent, nack received
      modulo_twi_error = TW_MT_DATA_NACK;
      modulo_twi_stop();
      break;
    case TW_MT_ARB_LOST: // lost bus arbitration
      modulo_twi_error = TW_MT_ARB_LOST;
      modulo_twi_releaseBus();
      break;

    // Master Receiver
    case TW_MR_DATA_ACK: // data received, ack sent
      // put byte into buffer
      modulo_twi_masterBuffer[modulo_twi_masterBufferIndex++] = TWDR;
    case TW_MR_SLA_ACK:  // address sent, ack received
      // ack if more bytes are expected, otherwise nack
      if(modulo_twi_masterBufferIndex < modulo_twi_masterBufferLength){
        modulo_twi_reply(1);
      }else{
        modulo_twi_reply(0);
      }
      break;
    case TW_MR_DATA_NACK: // data received, nack sent
      // put final byte into buffer
      modulo_twi_masterBuffer[modulo_twi_masterBufferIndex++] = TWDR;
	if (modulo_twi_sendStop)
          modulo_twi_stop();
	else {
	  modulo_twi_inRepStart = true;	// we're gonna send the START
	  // don't enable the interrupt. We'll generate the start, but we
	  // avoid handling the interrupt until we're in the next transaction,
	  // at the point where we would normally issue the start.
	  TWCR = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN) ;
	  modulo_twi_state = MODULO_TWI_READY;
	}
	break;
    case TW_MR_SLA_NACK: // address sent, nack received
      modulo_twi_stop();
      break;
    // TW_MR_ARB_LOST handled by TW_MT_ARB_LOST case

    // Slave Receiver
    case TW_SR_SLA_ACK:   // addressed, returned ack
    case TW_SR_GCALL_ACK: // addressed generally, returned ack
    case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
    case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
      // enter slave receiver mode
      modulo_twi_state = MODULO_TWI_SRX;
      // indicate that rx buffer can be overwritten and ack
      modulo_twi_rxBufferIndex = 0;
      modulo_twi_reply(1);
      break;
    case TW_SR_DATA_ACK:       // data received, returned ack
    case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
      // if there is still room in the rx buffer
      if(modulo_twi_rxBufferIndex < MODULO_TWI_BUFFER_LENGTH){
        // put byte in buffer and ack
        modulo_twi_rxBuffer[modulo_twi_rxBufferIndex++] = TWDR;
        modulo_twi_reply(1);
      }else{
        // otherwise nack
        modulo_twi_reply(0);
      }
      break;
    case TW_SR_STOP: // stop or repeated start condition received
      // put a null char after data if there's room
      if(modulo_twi_rxBufferIndex < MODULO_TWI_BUFFER_LENGTH){
        modulo_twi_rxBuffer[modulo_twi_rxBufferIndex] = '\0';
      }
      // sends ack and stops interface for clock stretching
      modulo_twi_stop();
      // callback to user defined callback
      modulo_twi_onSlaveReceive(modulo_twi_rxBuffer, modulo_twi_rxBufferIndex);
      // since we submit rx buffer to "wire" library, we can reset it
      modulo_twi_rxBufferIndex = 0;
      // ack future responses and leave slave receiver state
      modulo_twi_releaseBus();
      break;
    case TW_SR_DATA_NACK:       // data received, returned nack
    case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
      // nack back at master
      modulo_twi_reply(0);
      break;

    // Slave Transmitter
    case TW_ST_SLA_ACK:          // addressed, returned ack
    case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
      // enter slave transmitter mode
      modulo_twi_state = MODULO_TWI_STX;
      // ready the tx buffer index for iteration
      modulo_twi_txBufferIndex = 0;
      // set tx buffer length to be zero, to verify if user changes it
      modulo_twi_txBufferLength = 0;
      // request for txBuffer to be filled and length to be set
      // note: user must call modulo_twi_transmit(bytes, length) to do this
      modulo_twi_onSlaveTransmit();
      // if they didn't change buffer & length, initialize it
      if(0 == modulo_twi_txBufferLength){
        modulo_twi_txBufferLength = 1;
        modulo_twi_txBuffer[0] = 0x00;
      }
      // transmit first byte from buffer, fall
    case TW_ST_DATA_ACK: // byte sent, ack returned
      // copy data to output register
      TWDR = modulo_twi_txBuffer[modulo_twi_txBufferIndex++];
      // if there is more to send, ack, otherwise nack
      if(modulo_twi_txBufferIndex < modulo_twi_txBufferLength){
        modulo_twi_reply(1);
      }else{
        modulo_twi_reply(0);
      }
      break;
    case TW_ST_DATA_NACK: // received nack, we are done
    case TW_ST_LAST_DATA: // received ack, but we are done already!
      // ack future responses
      modulo_twi_reply(1);
      // leave slave receiver state
      modulo_twi_state = MODULO_TWI_READY;
      break;

    // All
    case TW_NO_INFO:   // no state information
      break;
    case TW_BUS_ERROR: // bus error, illegal stop/start
      modulo_twi_error = TW_BUS_ERROR;
      modulo_twi_stop();
      break;
  }

}

#endif
