/*
 * servo.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include <stdlib.h>
#include "servo.h"

CServo::CServo(ServoData data) {
	m_pinM1 = data.pinM1;
	m_pinM2 = data.pinM2;
	this->enablePwmFunPtr = data.enablePwmFunPtr;
	m_timer1Obj = data.timer1Obj;
}

CServo::~CServo() {
	// TODO Auto-generated destructor stub
}

//Setters
void CServo::setEnablePwmFunPtr(void (CTimer1::*enablePwmFunPtr)(uint8_t))
{
	this->enablePwmFunPtr = enablePwmFunPtr;
}

void CServo::setMaxEncoderCounter(uint64_t value)
{
	m_maxEncoderCounter = value;
}

void CServo::setSingleEncoderStep(uint8_t value)
{
	m_singleEncoderStep = value;
}

void CServo::setBlindType(BlindType blindType)
{
	m_blindType = blindType;
}

void CServo::setDirection(CServo::Direction dir)
{
	m_dir = dir;
}

void CServo::setOpenPercent(uint8_t value)
{
	m_nOpenPercent = value;
}

void CServo::setSpeedPercent(uint8_t value)
{
	m_nSpeedPercent = value;
}

void CServo::setAutoDrive(bool value)
{
	m_bAutoDrive = value;
}

void CServo::setVisibility(Visibility vis)
{
	m_visibility = vis;
}

void CServo::setWindowClosed(bool value)
{
	m_bIsWindowClosed = value;
}

//Getters
const int64_t &CServo::getMaxEncoderCounter() const
{
	return m_maxEncoderCounter;
}

CServo::BlindType CServo::getblindType()
{
	return m_blindType;
}


CServo::Direction CServo::getDirection()
{
	return m_dir;
}

uint8_t CServo::getOpenPercent()
{
	return m_nOpenPercent;
}

uint8_t CServo::getSpeedPercent()
{
	return m_nSpeedPercent;
}

bool CServo::isAutoDrive()
{
	return m_bAutoDrive;
}

CServo::Visibility CServo::getVisibility()
{
	return m_visibility;
}

bool CServo::isWindowClosed()
{
	return m_bIsWindowClosed;
}


//Operations
void CServo::runClockwise(uint8_t speed)
{
	IO::Port::Set(m_pinM1.port, m_pinM1.pin, IO::state::high);
	IO::Port::Set(m_pinM2.port, m_pinM2.pin, IO::state::low);
	processSpeed(speed);
}

void CServo::runCounterClockise(uint8_t speed)
{
	IO::Port::Set(m_pinM1.port, m_pinM1.pin, IO::state::low);
	IO::Port::Set(m_pinM2.port, m_pinM2.pin, IO::state::high);
	processSpeed(speed);
}

void CServo::stop()
{
	IO::Port::Set(m_pinM1.port, m_pinM1.pin, IO::state::low);
	IO::Port::Set(m_pinM2.port, m_pinM2.pin, IO::state::low);
	processSpeed(0);
}

void CServo::processSpeed(uint8_t percent)
{
	if(percent > 100)
		percent = 100;
	(m_timer1Obj->*enablePwmFunPtr)((percent * 255) / 100);
}

void CServo::processPercentOfOpen(uint8_t percent, Visibility vis)
{
	m_nOpenPercent = percent;
	uint64_t temp = (percent / 100);
	if(m_blindType == BlindType::FullBlackout)
	{
		m_setEncoderCounter = m_maxEncoderCounter * temp;
		return;
	}
	else
	{
		//Here Day/Night alghoritm becomes
		uint64_t setVal = m_maxEncoderCounter * temp;
		if(setVal == 0)
		{
			m_setEncoderCounter =0;
			return;
		}
		else
		{
			//Check Type
			uint8_t startValue;
			if(vis == Visibility::Day)
				startValue = 2;
			else if(vis == Visibility::Night)
				startValue = 1;
			else
			{
				m_setEncoderCounter = 0;
				return;
			}

			//Prepare temp value
			uint64_t tempVal = 0;
			for(uint8_t i = startValue; i <= m_maxEncoderCounter; i += 2)
			{
				uint64_t realVal = i * m_singleEncoderStep;
				uint64_t diffVal = abs(realVal - setVal);
				if(diffVal < tempVal)
					break;
				else
					tempVal = diffVal;
			}
			m_setEncoderCounter = tempVal;
		}
	}
}

void CServo::processRunServo(CServo::Direction dir, uint8_t speed)
{
	if(dir == Direction::Open)
	{
		runClockwise(speed);
	}
	else if(dir == Direction::Close)
	{
		runCounterClockise(speed);
	}
	else
	{
		stop();
	}
}
