/*
 * ADC.h
 *
 *  Created on: 22.03.2017
 *      Author: Damian
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

class CADC {
	//Private members
private:
	uint32_t m_u32_averageADC;
	uint8_t m_u8_adcPin;
	uint8_t u8_dt;

	//Public interface
public:
	CADC();
	~CADC();

	void initADC(uint8_t u8_pin);
	uint16_t measureADC();
	uint16_t averageADC();
	uint8_t getDT(void);
};

#endif /* ADC_H_ */
