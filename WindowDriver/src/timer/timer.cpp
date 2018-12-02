/*
 * timer.cpp
 *
 *  Created on: 07.03.2017
 *      Author: Damian
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include "timer.h"
#include "../io/io.h"

/**
 * Member method definitions for TIMER1
 */

bool CTimer1::isCreated = 0;
CTimer1 * CTimer1::timerInstance = nullptr;

CTimer1 * CTimer1::getInstance()
{
	if(!isCreated)
	{
		static CTimer1 timer;
		timerInstance = &timer;
		isCreated = 1;
	}
	return timerInstance;
}

CTimer1::CTimer1()
{

	IO::Dir::Set(&DDRD, PD4, IO::direction::out);
	IO::Port::Set(&PORTD, PD4, IO::state::low);

	IO::Dir::Set(&DDRD, PD5, IO::direction::out);
	IO::Port::Set(&PORTD, PD5, IO::state::low);

	//Initialize Timer1 into FastPwm mode
	TCCR1A |= (1<<WGM10);
	TCCR1B |= (1<<WGM12);	//FastPWM mode
	TCCR1A |= (1<<COM1A1); 	//set at bottom, clear on compare match
	TCCR1A |= (1<<COM1B1);	//set at bottom, clear at compare match
	TCCR1B |= (1<<CS11) | (1<<CS10);

	OCR1A = 0;
	OCR1B = 0;
}

CTimer1::~CTimer1()
{

}

void CTimer1::setChannel(uint8_t channel, uint8_t compareValue)
{
	if(channel == 1)
		OCR1A = compareValue;
	else if(channel == 2)
		OCR1B = compareValue;
}

void CTimer1::setChannelA(uint8_t compareValue)
{
	OCR1A = compareValue;
}

void CTimer1::setChannelB(uint8_t compareValue)
{
	OCR1B = compareValue;
}


/**
 * Member methods definitions for TIMER2
 */

volatile CTimer2::TPROGTIMER CTimer2::TProgTimer[8];
bool CTimer2::isCreated = 0;
CTimer2 * CTimer2::timerInstance = nullptr;

CTimer2 * CTimer2::getInstance(T2Prescallers prescaller, uint8_t Tick)
{
	if(!isCreated)
	{
		static CTimer2 timer(prescaller, Tick);
		timerInstance = &timer;
		isCreated = 1;
	}
	return timerInstance;
}

CTimer2::CTimer2(T2Prescallers prescaller, uint8_t Tick)
{
	//Initialize all elemets
	for(uint8_t i = 0; i < 8; ++i)
	{
		TProgTimer[i].b_Active = false;
		TProgTimer[i].u16_Interval = 0;
		TProgTimer[i].u16_Counter = 0;
		TProgTimer[i].pFunct = nullptr;
	}
	//Initialize Timer2
	TCCR2 |= (1<<WGM21);			//Choosen mode
	TCCR2 |= prescaller;	//Choosen prescaller
	OCR2 = Tick;			//Additionally divide
	TIMSK |= (1 << OCIE2);


}

CTimer2::~CTimer2()
{

}

void CTimer2::Assign(uint8_t HWNDnumber, uint16_t Interval, void (*pfun)(void), bool state)
{
	TProgTimer[HWNDnumber].u16_Interval = Interval;
	TProgTimer[HWNDnumber].u16_Counter = 0;
	TProgTimer[HWNDnumber].b_Active = state;
	TProgTimer[HWNDnumber].pFunct = pfun;
}

int8_t CTimer2::AssignFreeChannel(uint16_t Interval, void (*pfun)(void), bool state)
{
	int8_t result = -1;
	for(uint8_t i = 0; i < 8; ++i)
	{
		if(!TProgTimer[i].pFunct)
		{
			TProgTimer[i].u16_Interval = Interval;
			TProgTimer[i].u16_Counter = 0;
			TProgTimer[i].b_Active = state;
			TProgTimer[i].pFunct = pfun;
			result = i;
			break;
		}
	}
	return result;
}

void CTimer2::UnAssign(uint8_t HandlerNumber)
{
	TProgTimer[HandlerNumber].u16_Interval = 0;
	TProgTimer[HandlerNumber].u16_Counter = 0;
	TProgTimer[HandlerNumber].b_Active = false;
	TProgTimer[HandlerNumber].pFunct = nullptr;
}

void CTimer2::Disable(uint8_t HandlerNumber) {
	TProgTimer[HandlerNumber].b_Active = false;
}

void CTimer2::Enable(uint8_t HandlerNumber) {
	TProgTimer[HandlerNumber].b_Active = true;
	TProgTimer[HandlerNumber].u16_Counter = 0;
}

void CTimer2::Hold (uint8_t HandlerNumber) {
	TProgTimer[HandlerNumber].b_Active = false;
}

void CTimer2::UnHold (uint8_t HandlerNumber) {
	TProgTimer[HandlerNumber].b_Active = true;
}

bool CTimer2::Enabled (uint8_t HandlerNumber) {
	return TProgTimer[HandlerNumber].b_Active;
}

void CTimer2::SetInterval (uint8_t HandlerNumber, uint64_t Interval) {
	TProgTimer[HandlerNumber].u16_Interval = Interval;
}

uint16_t CTimer2::GetInterval (uint8_t HandlerNumber) {
	return TProgTimer[HandlerNumber].u16_Interval;
}

void CTimer2::SetPrescaller (T2Prescallers Prescaller) {
	TCCR2 &= ~((1 << CS00) | (1 << CS01) | (CS02));
	TCCR2 |= Prescaller;
	TCCR2 |= (1<<WGM21);			//Choosen mode
}

void CTimer2::SetPeriod (uint8_t Tick) {
	OCR2 = Tick;
}

bool CTimer2::isTimerActive(uint8_t HandlerNumber)
{
	if(TProgTimer[HandlerNumber].b_Active)
		return 1;
	else
		return 0;
}

void TIMER2_COMP_vect(void)
{

	for(uint8_t i = 0; i < 8; ++i)
		{
			if(CTimer2::timerInstance->TProgTimer[i].b_Active && CTimer2::timerInstance->TProgTimer[i].pFunct != nullptr)
			{

				if( !(CTimer2::timerInstance->TProgTimer[i].u16_Counter)-- )
				{

					CTimer2::timerInstance->TProgTimer[i].u16_Counter = CTimer2::timerInstance->TProgTimer[i].u16_Interval;
					CTimer2::timerInstance->TProgTimer[i].pFunct();
				}
			}
		}
}


/*
//Here is interrupt event for dedinied timer
ISR(TIMER2_COMP_vect)
{

	static uint8_t n;
	n = CTimer2::timer;
	if(n) CTimer2::timer = --n;

	for(uint8_t i = 0; i < 8; ++i)
	{
		if(CTimer2::TProgTimer[i].b_Active && CTimer2::TProgTimer[i].pFunct != nullptr)
		{
			if(CTimer2::TProgTimer[i].u16_Counter == CTimer2::TProgTimer[i].u16_Interval)
			{
				CTimer2::TProgTimer[i].u16_Counter = 0;
				CTimer2::TProgTimer[i].pFunct();
			}
			else
				CTimer2::TProgTimer[i].u16_Counter++;
		}
	}
}
*/

