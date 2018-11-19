/*
 * quadratureEncoder.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: damian
 */


#include "quadratureEncoder.h"

#include "../io/io.h"

// *** PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
// *** PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
// *** DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)

//Create global variable for encoder 1
CQuadratureEncoder encoderForServo1({
	(1 << INT0),
	(1 << ISC00) | (1 << ISC01),
	{	&DDRD,
		&PORTD,
		&PIND,
		PD2
	},
	{	&DDRD,
		&PORTD,
		&PIND,
		PD0
	}
});

//Create global variable for encoder 2
CQuadratureEncoder encoderForServo2({
	(1 << INT1),
	(1 << ISC10),
	{	&DDRD,
		&PORTD,
		&PIND,
		PD3
	},
	{	&DDRD,
		&PORTD,
		&PIND,
		PD1
	}
});

/*
 * Class implementation
 */
CQuadratureEncoder::CQuadratureEncoder(EncoderSettings settings)
{
	//Assing pins
	m_pin1 = settings.pin1;
	m_pin2 = settings.pin2;


	//Setup pins
	IO::Dir::Set(m_pin1.ddr, m_pin1.pin, IO::direction::in);
	IO::Dir::Set(m_pin2.ddr, m_pin2.pin, IO::direction::in);

	IO::Port::Set(m_pin1.port, m_pin1.pin, IO::state::high);
	IO::Port::Set(m_pin2.port, m_pin2.pin, IO::state::high);

	//Setup external interrupt for any logical change on pin
	GICR |= settings.gicr;
	MCUCR |= settings.mcucr;
}

CQuadratureEncoder::~CQuadratureEncoder() {
	// TODO Auto-generated destructor stub
}

void CQuadratureEncoder::processGrayCode()
{
	//PORTB ^= (1 << PB7);

	if(IO::Pin::isSet(m_pin2.pinX, m_pin2.pin))
	{
		//PORTB ^= (1 << PB5);
		m_counter++;
	}
	else
	{
		//PORTB ^= (1 << PB5);
		m_counter--;
	}
}

int32_t CQuadratureEncoder::getCounter()
{
	return m_counter;
}

void CQuadratureEncoder::resetCounter()
{
	m_counter = 0;
}

void INT0_vect(void)
{
	encoderForServo1.processGrayCode();
}

void INT1_vect(void)
{
	encoderForServo2.processGrayCode();
}


