/*
 * IREncoding.h
 *
 * Created: 3/30/2015 10:09:58 PM
 *  Author: ekt
 *
 * This uses some timing constants from the IRRemote arduino library.
 * https://github.com/z3t0/Arduino-IRremote
 */ 


#ifndef IRENCODING_H_
#define IRENCODING_H_

#include <inttypes.h>



// Most IR Protocols use either pulse width or pulse distance encoding
// and vary primarily in the header pulses, specific timing, number of bits.
// PulseModulationEncoding allows a protocol's encoding to be specified
// in terms of those details.
struct PulseModulationEncoding {
    uint16_t protocol;      // The protocol number (see below)
    uint16_t headerMark;    // Length of the header mark in us
    uint16_t headerSpace;   // Length of the header space in us
    uint16_t numBits;       // Number of bits
    uint16_t oneMark;       // Length of the mark for a "1" bit
    uint16_t oneSpace;      // Length of the space for a "1" bit
    uint16_t zeroMark;      // Length of the mark for a "0" bit
    uint16_t zeroSpace;     // Length of the space for a "0" bit
    uint16_t stopMark;      // Length of the trailing "stop" mark
    uint16_t khz;           // Modulation frequency in kHz
};

#define NUM_IR_ENCODINGS 7

#define IR_PROTOCOL_NEC 0
#define IR_PROTOCOL_NEC_REPEAT 1
#define IR_PROTOCOL_SONY 2
#define IR_PROTOCOL_JVC 3
#define IR_PROTOCOL_JVC_REPEAT 4
#define IR_PROTOCOL_RC5 5
#define IR_PROTOCOL_RC6 6
#define IR_PROTOCOL_PANASONIC 7
#define IR_PROTOCOL_LG 8

extern const PulseModulationEncoding irEncodings[NUM_IR_ENCODINGS];


bool IRDecode(uint16_t *rawData, uint16_t rawLen,
    int8_t *protocol, uint32_t *value);


#endif /* IRENCODING_H_ */
