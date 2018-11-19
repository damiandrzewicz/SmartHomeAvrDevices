/*
 * ADC.cpp


 *
 *  Created on: 22.03.2017
 *      Author: Damian
 */

#include "../ADC/ADC.h"

CADC::CADC()
{
	m_u32_averageADC = 0;
	m_u8_adcPin = 0;
	u8_dt = 30;
}

CADC::~CADC() {

}

uint8_t CADC::getDT(void)
{
	return u8_dt;
}

void CADC::initADC(uint8_t u8_pin)
{
	m_u8_adcPin = u8_pin;
	//Set ADC
	ADMUX |= (1<<REFS1) | (1<<REFS0);	//ref 2.56V
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//ADC enable, prescaler = 128 (108kHz)

}

uint16_t CADC::measureADC()
{
	//set selected channel in ADCMUX register
	ADMUX = (ADMUX & 0xF8) | m_u8_adcPin;
	//start measure
	ADCSRA |= (1<<ADSC);
	//waiting for measure finish
	while(ADCSRA & (1<<ADSC));
	//return measured value
	return ADCW;
}

uint16_t CADC::averageADC()
{
	uint16_t u16_result;

	u16_result = measureADC();

	//Set average value
	m_u32_averageADC = m_u32_averageADC * u8_dt;
	m_u32_averageADC = m_u32_averageADC + u16_result;
	m_u32_averageADC = (m_u32_averageADC / (u8_dt+1));

	return m_u32_averageADC;
}

