/*
 * servo.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include <stdlib.h>
#include "servo.h"

CServo::CServo(ServoData data, uint8_t nBlindNo)
	: m_servoModel(nBlindNo)
{
	m_pinM1 = data.pinM1;
	m_pinM2 = data.pinM2;
	this->enablePwmFunPtr = data.enablePwmFunPtr;
	m_timer1Obj = data.timer1Obj;
}

CServo::~CServo() {
	// TODO Auto-generated destructor stub
}

CServoModel *CServo::getServoModel()
{
	return &m_servoModel;
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

//Getters
const int64_t &CServo::getMaxEncoderCounter() const
{
	return m_maxEncoderCounter;
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

void CServo::processPercentOfOpen(uint8_t percent, WindowData::Visibility vis)
{
	m_servoModel.setOpenPercent(percent);
	uint64_t temp = (percent / 100);
	if(m_servoModel.getBlindMetadataObject().blindType == WindowData::BlindType::FullBlackout)
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
			if(vis == WindowData::Visibility::Day)
				startValue = 2;
			else if(vis == WindowData::Visibility::Night)
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

void CServo::processRunServo(WindowData::Direction dir, uint8_t speed)
{
	if(dir == WindowData::Direction::Open)
	{
		runClockwise(speed);
	}
	else if(dir == WindowData::Direction::Close)
	{
		runCounterClockise(speed);
	}
	else
	{
		stop();
	}
}
