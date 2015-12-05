/*
 * IREncoding.cpp
 *
 * Created: 3/30/2015 10:10:17 PM
 *  Author: ekt
 */

#include "IREncoding.h"

#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1690
#define NEC_ZERO_SPACE	560
#define NEC_RPT_SPACE	2250

#define SONY_HDR_MARK	2400
#define SONY_HDR_SPACE	600
#define SONY_ONE_MARK	1200
#define SONY_ZERO_MARK	600
#define SONY_RPT_LENGTH 45000
#define SONY_DOUBLE_SPACE_USECS  500  // usually ssee 713 - not using ticks as get number wrapround

#define JVC_HDR_MARK 8000
#define JVC_HDR_SPACE 4000
#define JVC_BIT_MARK 600
#define JVC_ONE_SPACE 1600
#define JVC_ZERO_SPACE 550
#define JVC_RPT_LENGTH 60000

#define PANASONIC_HDR_MARK 3502
#define PANASONIC_HDR_SPACE 1750
#define PANASONIC_BIT_MARK 502
#define PANASONIC_ONE_SPACE 1244
#define PANASONIC_ZERO_SPACE 400

#define LG_HDR_MARK 8000
#define LG_HDR_SPACE 4000
#define LG_BIT_MARK 600
#define LG_ONE_SPACE 1600
#define LG_ZERO_SPACE 550
#define LG_RPT_LENGTH 60000


#define NEC_BITS 32
#define SONY_BITS 12
#define SANYO_BITS 12
#define MITSUBISHI_BITS 16
#define MIN_RC5_SAMPLES 11
#define MIN_RC6_SAMPLES 1
#define PANASONIC_BITS 48
#define JVC_BITS 16
#define LG_BITS 28
#define SAMSUNG_BITS 32
#define WHYNTER_BITS 32


const PulseModulationEncoding IREncodings[NUM_IR_ENCODINGS] = {

	// NEC
	{
		.protocol = IR_PROTOCOL_NEC,
		.headerMark = NEC_HDR_MARK,
		.headerSpace = NEC_HDR_SPACE,
		.numBits = 32,
		.oneMark = NEC_BIT_MARK,
		.oneSpace = NEC_ONE_SPACE,
		.zeroMark = NEC_BIT_MARK,
		.zeroSpace = NEC_ZERO_SPACE,
		.stopMark = NEC_BIT_MARK,
		.khz = 38
	},

	// Special NEC Repeat Codes
	{
		.protocol = IR_PROTOCOL_NEC_REPEAT,
		.headerMark = NEC_HDR_MARK,
		.headerSpace = NEC_RPT_SPACE,
		.numBits = 0,
		.oneMark = NEC_BIT_MARK,
		.oneSpace = NEC_ONE_SPACE,
		.zeroMark = NEC_BIT_MARK,
		.zeroSpace = NEC_ZERO_SPACE,
		.stopMark = NEC_BIT_MARK,
		.khz = 38
	},

	{
		.protocol = IR_PROTOCOL_SONY,
		.headerMark = SONY_HDR_MARK,
		.headerSpace = SONY_HDR_SPACE,
		.numBits = SONY_BITS,
		.oneMark = SONY_ONE_MARK,
		.oneSpace = SONY_HDR_SPACE,
		.zeroMark = SONY_ZERO_MARK,
		.zeroSpace = SONY_HDR_SPACE,
		.stopMark = 0,
		.khz = 40
	},

	{
		.protocol = IR_PROTOCOL_JVC,
		.headerMark = JVC_HDR_MARK,
		.headerSpace = JVC_HDR_SPACE,
		.numBits = JVC_BITS,
		.oneMark = JVC_BIT_MARK,
		.oneSpace = JVC_ONE_SPACE,
		.zeroMark = JVC_BIT_MARK,
		.zeroSpace = JVC_ZERO_SPACE,
		.stopMark = JVC_BIT_MARK,
		.khz = 38
	},

	{
		.protocol = IR_PROTOCOL_JVC_REPEAT,
		.headerMark = 0, // No Header for repeat codes
		.headerSpace = 0, 
		.numBits = JVC_BITS,
		.oneMark = JVC_BIT_MARK,
		.oneSpace = JVC_ONE_SPACE,
		.zeroMark = JVC_BIT_MARK,
		.zeroSpace = JVC_ZERO_SPACE,
		.stopMark = JVC_BIT_MARK,
		.khz = 38
	},

	{
		.protocol = IR_PROTOCOL_PANASONIC,
		.headerMark = PANASONIC_HDR_MARK,
		.headerSpace = PANASONIC_HDR_SPACE,
		.numBits = PANASONIC_BITS,
		.oneMark = PANASONIC_BIT_MARK,
		.oneSpace = PANASONIC_ONE_SPACE,
		.zeroMark = PANASONIC_BIT_MARK,
		.zeroSpace = PANASONIC_ZERO_SPACE,
		.stopMark = PANASONIC_BIT_MARK,
		.khz = 35
	},

	 {
		.protocol = IR_PROTOCOL_LG,
		.headerMark = LG_HDR_MARK, 
		.headerSpace = LG_HDR_SPACE,
		.numBits = LG_BITS,
		.oneMark = LG_BIT_MARK,
		.oneSpace = LG_ONE_SPACE,
		.zeroMark = LG_BIT_MARK,
		.zeroSpace = LG_ZERO_SPACE,
		.stopMark = LG_BIT_MARK,
		.khz = 38
	},

};


#include <inttypes.h>

#include "IREncoding.h"

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define MARK_EXCESS 0

#define LTOL 3
#define UTOL 5

#define TICKS_LOW(us) (int) (((us)*LTOL/(4*USECPERTICK)))
#define TICKS_HIGH(us) (int) (((us)*UTOL/(4*USECPERTICK) + 1))

#define MIN_RC5_SAMPLES     11
#define RC5_T1             889

#define MIN_RC6_SAMPLES      1
#define RC6_HDR_MARK      2666
#define RC6_HDR_SPACE      889
#define RC6_T1             444
#define RC6_RPT_LENGTH   46000

class IRDecoder {
public:
	IRDecoder(uint16_t *rawData, uint16_t rawLen) :
		_rawData(rawData), _rawLen(rawLen) {}
	
	bool decodePulseModulation(const PulseModulationEncoding &encoding, uint32_t *value)
	{
		_currentIndex = 0;
		*value = 0;
		
		// Skip leading space
		getNextInterval();

		// Match the header
		if (encoding.headerMark) {
			if (!matchMark(getNextInterval(), encoding.headerMark)) {
				return false;
			}

			if (!matchSpace(getNextInterval(), encoding.headerSpace)) {
				return false;
			}
		}

		// Match the data
		for (int i = 0; i < (int)encoding.numBits; i++) {
			int16_t mark = getNextInterval();
			int16_t space = getNextInterval();
			
			if (matchMark(mark, encoding.oneMark) and
				matchSpace(space, encoding.oneSpace)) {
				*value = (*value << 1) | 1;
			} else if (matchMark(mark, encoding.zeroMark) and
					   matchSpace(space, encoding.zeroSpace)) {
				*value <<= 1;
			} else {
				return false;
			}
		}

		// Match the stop mark
		if (encoding.stopMark) {
			if (!matchMark(getNextInterval(), encoding.stopMark)) {
				return false;
			}
		}

		// Success!
		return true;
	}
	
	// Specialized decoding of the RC5 protocol.
	// In the future it would be nice to have a general manchester decoder.
	bool decodeRC5(uint32_t *value)
	{
		long  data   = 0;
		int   used   = 0;
		_currentIndex  = 1;  // Skip gap space

		if (_rawLen < MIN_RC5_SAMPLES + 2)
			return false;

		// Get start bits
		if (getRClevel(&used, RC5_T1) != true)
			return false ;
		if (getRClevel(&used, RC5_T1) != false)
			return false ;
		if (getRClevel(&used, RC5_T1) != true)
			return false ;

		for (int nbits = 0;  _currentIndex < _rawLen;  nbits++) {
			int  levelA = getRClevel(&used, RC5_T1);
			int  levelB = getRClevel(&used, RC5_T1);

			if ((levelA == false) && (levelB == true ))
				data = (data << 1) | 1 ;
			else if ((levelA == true ) && (levelB == false))
				data = (data << 1) | 0 ;
			else
				return false ;
		}

		// Success
		*value = data;
		return true;
	}

	// Specialized decoding of the RC6 protocol.
	// In the future it would be nice to have a general manchester decoder.
	bool decodeRC6(uint32_t *value)
	{
		long  data   = 0;
		int   used   = 0;
		_currentIndex = 1;  // Skip first space

		if (_rawLen < MIN_RC6_SAMPLES)
			return false;

		// Initial mark
		if (!matchMark(_rawData[_currentIndex++],  RC6_HDR_MARK))
			return false;

		if (!matchSpace(_rawData[_currentIndex++], RC6_HDR_SPACE))
			return false;

		// Get start bit (1)
		if (getRClevel(&used, RC6_T1) != true)
			return false;

		if (getRClevel(&used, RC6_T1) != false)
			return false;

		for (int nbits = 0;  _currentIndex < _rawLen;  nbits++) {
			int  levelA, levelB;  // Next two levels

			levelA = getRClevel(&used, RC6_T1);
			if (nbits == 3) {
				// T bit is double wide; make sure second half matches
				if (levelA != getRClevel(&used, RC6_T1)) return false;
			}

			levelB = getRClevel(&used, RC6_T1);
			if (nbits == 3) {
				// T bit is double wide; make sure second half matches
				if (levelB != getRClevel(&used, RC6_T1)) return false;
			}

			if ((levelA == true ) && (levelB == false))
				data = (data << 1) | 1 ;  // inverted compared to RC5
			else if ((levelA == false) && (levelB == true ))
				data = (data << 1) | 0 ;  // ...
			else
				return false ;            // Error
		}

		// Success
		*value = data;
		return true;
	}

private:

	bool match(int16_t measured, int16_t desired_us) {
		return (measured >= TICKS_LOW(desired_us) and measured <= TICKS_HIGH(desired_us));
	}
	
	bool matchMark(int16_t measured, int16_t desired_us) {
		return match(measured, desired_us+MARK_EXCESS);
	}
	
	bool matchSpace(int16_t measured, int16_t desired_us) {
		return match(measured, desired_us-MARK_EXCESS);
	}
	
	int16_t getNextInterval() {
		// Return 0 if we have gone off the end of the data
		if (_currentIndex >= _rawLen) {
			return 0;
		}
		
		// Get the next interval
		int16_t next = _rawData[_currentIndex++];
		
		// 0 is a special token indicating that we should read a 16 bit value
		if (next == 0) {
			next = _rawData[_currentIndex++];
			next |= (_rawData[_currentIndex++] << 8);
		}
		return next;
	}
	
	// Gets one undecoded level at a time from the raw buffer.
	// The RC5/6 decoding is easier if the data is broken into time intervals.
	// E.g. if the buffer has MARK for 2 time intervals and SPACE for 1,
	// successive calls to getRClevel will return MARK, MARK, SPACE.
	// offset and used are updated to keep track of the current position.
	// t1 is the time interval for a single bit in microseconds.
	// Returns -1 for error (measured time interval is not a multiple of t1).
	//
	int getRClevel(int *used, int t1)
	{
		if (_currentIndex >= _rawLen)
			return false ;  // After end of recorded buffer, assume SPACE.
		int width      = _rawData[_currentIndex];
		int val        = (_currentIndex % 2);
		int correction = val ? MARK_EXCESS : - MARK_EXCESS;

		int avail = 0;
		if (match(width, (  t1) + correction))
			avail = 1 ;
		else if (match(width, (2*t1) + correction))
			avail = 2 ;
		else if (match(width, (3*t1) + correction))
			avail = 3 ;
		else
			return -1 ;

		(*used)++;
		if (*used >= avail) {
			*used = 0;
			_currentIndex++;
		}

		return val;
	}


private:
	int16_t _currentIndex;	
	uint16_t *_rawData;
	uint16_t _rawLen;
};

bool IRDecode(uint16_t *rawData, uint16_t rawLen, int8_t *protocol, uint32_t *value) {
	IRDecoder decoder(rawData, rawLen);
	
	if (decoder.decodeRC5(value)) {
		*protocol = IR_PROTOCOL_RC5;
		return true;
	}

	if (decoder.decodeRC6(value)) {
		*protocol = IR_PROTOCOL_RC6;
		return true;
	}

	for (int i=0; i < NUM_IR_ENCODINGS ; i++) {
		if (decoder.decodePulseModulation(IREncodings[i], value)) {
			*protocol = IREncodings[i].protocol;
			return true;
		}
	}

	return false;
}

static int encodePulseModulation(PulseModulationEncoding &encoding,
	uint32_t data, uint8_t *rawData, uint8_t maxLen) {

	// Index 0 is a space. Set it to 0 length.
	rawData[0] = 0;
	int length = 1;
	
	if (encoding.headerMark and length+2 < maxLen) {
		rawData[length++] = encoding.headerMark/USECPERTICK;
		rawData[length++] = encoding.headerSpace/USECPERTICK;
	}

	for (int i=0; i < encoding.numBits and length+2 < maxLen; i++) {
		if (data & (1 << i)) {
			rawData[length++] = encoding.oneMark/USECPERTICK;
			rawData[length++] = encoding.oneSpace/USECPERTICK;
		} else {
			rawData[length++] = encoding.zeroMark/USECPERTICK;
			rawData[length++] = encoding.zeroSpace/USECPERTICK;
		}
	}
	
	if (encoding.stopMark and length+2 < maxLen) {
		rawData[length++] = encoding.stopMark/USECPERTICK;
		rawData[length++] = 0;
	}
	
	return length;
}

class RCEncoder {

	RCEncoder(uint8_t *rawData, uint8_t maxLen) :
		_rawData(rawData), _maxLen(maxLen), _length(0) {}

	void encodeRC5(uint32_t data, uint8_t numBits) {
		_appendBit(1);
		_appendBit(1);
		
		for (int i=0; i < numBits; i++) {
			_appendBit(data & (1 << i));
		}
	}

	void encodeRC6(uint32_t data, uint8_t numBits) {
		// Leader symbol
		_appendInterval(true, RC6_HDR_MARK/USECPERTICK);
		_appendInterval(false, RC6_HDR_SPACE/USECPERTICK);
		
		// Start bit
		_appendBit(true);
		
		for (int i=0; i < numBits; i++) {
			// Bit 4 (the trailer/toggle bit) is twice as long.
			_appendBit(data & (1 << i), i == 4);
		}
	}


private:
	
	void _appendBit(bool bit, bool doubleLength = false) {
		_appendInterval(!bit, (doubleLength+1)*RC5_T1/USECPERTICK);
		_appendInterval(bit,  (doubleLength+1)*RC5_T1/USECPERTICK);
	}

	void _appendInterval(bool mark, uint8_t ticks) {
		bool lastIsMark = (_length % 2);

		if (mark == lastIsMark) {
			// If this interval and the previous one have the same state (both mark
			// or both space) then just add the length
			_rawData[_length-1] += ticks;
		} else {
			// If this interval and the previous have different states, then create
			// a new state.
			_rawData[_length++] = ticks;
		}
	}

	uint8_t *_rawData;
	uint8_t _maxLen;
	uint8_t _length;

};



#if 0
static void sendPulseModulation(PulseModulationEncoding &encoding, uint32_t data) {
	// Send the header
	if (encoding.headerMark) {
		sendMark(encoding.headerMark);
		sendSpace(encoding.headerSpace);
	}

	// Send the data
	for (int i = 0; i < (int)encoding.numBits; i++) {
		if (data & _BV(i)) {
			sendMark(encoding.oneMark);
			sendSpace(encoding.oneSpace);
			} else {
			sendMark(encoding.zeroMark);
			sendSpace(encoding.zeroSpace);
		}
	}

	// Send stop mark
	if (encoding.stopMark) {
		sendMark(encoding.stopMark);
	}
		
	// Return to idle
	sendSpace(0);
}
#endif


