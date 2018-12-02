/*
 * quadratureEncoder.h
 *
 *  Created on: Jun 11, 2018
 *      Author: damian
 */

#ifndef SRC_QUADRATUREENCODERS_H_
#define SRC_QUADRATUREENCODERS_H_

//#include <avr/io.h>
#include <avr/interrupt.h>

#include "../io/io.h"

extern "C"
{
	void INT0_vect(void) __attribute__ ((signal,used, externally_visible)) ;
	void INT1_vect(void) __attribute__ ((signal,used, externally_visible)) ;
}

struct EncoderSettings
{
	uint8_t gicr;
	uint8_t mcucr;
	IO::PinData pin1;
	IO::PinData pin2;
};

class CQuadratureEncoder {
public:
	CQuadratureEncoder(EncoderSettings settings);
	~CQuadratureEncoder();

	void setCounter(int32_t value);
	int32_t getCounter(bool bInvertedValue = false);
	void resetCounter();

	friend void INT0_vect (void);
	friend void INT1_vect (void);

private:
	void processGrayCode();

	//Members
private:
	IO::PinData m_pin1;
	IO::PinData m_pin2;

	//static const uint8_t m_sPulsesToIgnore = 10;

public:
	volatile int16_t m_counter = 0;
	uint8_t m_resolution = 0;
};

#endif /* SRC_QUADRATUREENCODERS_H_ */
