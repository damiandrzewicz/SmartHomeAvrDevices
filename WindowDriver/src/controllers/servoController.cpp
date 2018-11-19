/*
 * servoController.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include <stdlib.h>
#include "servoController.h"
#include "../utils/utils.h"

#include "../io/io.h"

#include "../uart/uart.h"

CServoController::CServoController() {


}

CServoController::~CServoController() {
	// TODO Auto-generated destructor stub
}

void CServoController::registerObjects(ServoData *pServoData, CQuadratureEncoder *pEncoder, CServoModel *pServoModel)
{
	m_pServoData = pServoData;
	initPins();

	m_pEncoder = pEncoder;
	m_pServoModel = pServoModel;
	m_nLastOpenPercentValue = m_pServoModel->getOpenPercent();

	//Init ADC
	m_adc.initADC(m_pServoData->pinADC.pin);
}

void CServoController::initPins()
{
	IO::Dir::Set(m_pServoData->pinM1.ddr, m_pServoData->pinM1.pin, IO::direction::out);
	IO::Dir::Set(m_pServoData->pinM2.ddr, m_pServoData->pinM2.pin, IO::direction::out);

	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::low);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::low);
}

void CServoController::processSpeed(uint8_t percent)
{
	if(percent > 100)
		percent = 100;
	(m_pServoData->timer1Obj->*m_pServoData->enablePwmFunPtr)((percent * 255) / 100);
}

//Operations
void CServoController::runClockwise(uint8_t speed)
{
	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::high);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::low);
	processSpeed(speed);
}

void CServoController::runCounterClockise(uint8_t speed)
{
	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::low);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::high);
	processSpeed(speed);
}

void CServoController::stop()
{
	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::low);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::low);
	processSpeed(0);
}

bool CServoController::checkIfIsInitialized()
{
	return (m_pServoModel->getBlindMetadataObject().blindType == WindowData::BlindType::None) ? false : true;
}

bool CServoController::isCalibrationActive()
{
	return (m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::Idle) ? false : true;
}

void CServoController::processCalibrate()
{
	static CBlindCalibrate::CalibrationStep lastStep = CBlindCalibrate::CalibrationStep::Idle;
	if(m_pServoModel->getCalibrateStep() != lastStep)
	{
		m_bCalibrationStepReady = true;
	}

	if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::ActivateCalibration && m_bCalibrationStepReady)
	{
		PORTB ^= (1 << PB7);
		CUart::getInstance()->puts("activ calibr\r\n");
		//Process step 1 - Activate calibration
		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::StartPoint && m_bCalibrationStepReady)
	{
		PORTB ^= (1 << PB7);
		//Save start point
		m_pEncoder->resetCounter();
		CUart::getInstance()->puts("start calibr\r\n");
		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::EndPoint && m_bCalibrationStepReady)
	{
		PORTB ^= (1 << PB7);
		m_pServoModel->getCalibrateMetadataObject().singleEncoderStep = m_pEncoder->getCounter() / 2;
		CUart::getInstance()->puts("endpt calibr\r\n");
		CUart::getInstance()->puts("singleEncoderStep=");
		CUart::getInstance()->putll(m_pServoModel->getCalibrateMetadataObject().singleEncoderStep, 10);
		CUart::getInstance()->puts("\r\n");
		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::EndPointFull && m_bCalibrationStepReady)
	{
		PORTB ^= (1 << PB7);

		m_pServoModel->getCalibrateMetadataObject().maxEncoderCounter =
				findClosestValue(m_pEncoder->getCounter(), m_pServoModel->getCalibrateMetadataObject().singleEncoderStep);

		CUart::getInstance()->puts("EndPointFull calibr\r\n");
		CUart::getInstance()->puts("maxEncoderCounter=");
		CUart::getInstance()->putll(m_pServoModel->getCalibrateMetadataObject().maxEncoderCounter, 10);
		CUart::getInstance()->puts("\r\n");

		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::Offset && m_bCalibrationStepReady)
	{
		PORTB ^= (1 << PB7);
		m_pServoModel->setManualDriveDirection(WindowData::Direction::Close);
		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == CBlindCalibrate::CalibrationStep::Finished
			&& m_bCalibrationStepReady
			&& m_nCurrentError)
	{
		PORTB ^= (1 << PB7);
		m_pServoModel->getCalibrateMetadataObject().offsetEncoder = m_pEncoder->getCounter();
		CUart::getInstance()->puts("Finished calibr\r\n");
		CUart::getInstance()->puts("offsetEncoder=");
		CUart::getInstance()->putll(m_pServoModel->getCalibrateMetadataObject().offsetEncoder, 10);
		CUart::getInstance()->puts("\r\n");



		m_bCalibrationStepReady = false;
	}

	lastStep = m_pServoModel->getCalibrateStep();
}

uint32_t CServoController::findClosestValue(uint32_t nMaxRange, uint32_t nSingleStep, uint32_t nSetValue, bool bUseSetValue)
{
   if(!nSingleStep)
		return 0;

	if(!bUseSetValue)
	{
		nSetValue = nMaxRange;
	}

	uint16_t i = 0;
	uint16_t step = 0;
	uint32_t diff = 999999;	//TODO assing max inteeger value
	while( (nSingleStep * i) <= (nMaxRange + nSingleStep) )
	{
		uint32_t tempDiff = abs( static_cast<int32_t>(nSetValue - (nSingleStep * i)) );
		if(tempDiff < diff)
		{
			diff = tempDiff;
			step = i;
		}
		i++;
	}

	return nSingleStep * step;
}

uint16_t CServoController::currentMeasure()
{
	uint16_t u16_average = m_adc.averageADC();

	uint16_t u16_voltage;
	uint16_t u16_current;
	uint32_t u16_temp;


	u16_temp = (u16_average + m_adc.getDT()) * 2560UL;
	u16_voltage = u16_temp / 1023;		//result in mV
	u16_current = ( u16_voltage * 1000UL ) / m_pServoData->adcResistorValue;

	return u16_current;
}

void CServoController::currentGuard()
{
	if(currentMeasure() > m_pServoData->adcMaxCurrent)
	{
		processRunServo(WindowData::Direction::Stop, 0);
		m_nCurrentError = true;
	}
}

void CServoController::encoderGuard()
{
//	if(m_nLastOpenPercentValue != m_pServoModel->getOpenPercent())
//	{
//
//	}

	if(m_pEncoder->getCounter() == m_nTargetEncoderValue)
	{
		processRunServo(WindowData::Direction::Stop, 0);
	}
}

void CServoController::processRunServo(WindowData::Direction dir, uint8_t speed)
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

void CServoController::processManualDrive()
{
	processRunServo(m_pServoModel->getManualDriveDirection(), 100);
}

bool CServoController::calculateTargetEncoderValue()
{
	uint32_t nOpenPercent = m_pServoModel->getOpenPercent();
	WindowData::Visibility vis = m_pServoModel->getBlindMetadataObject().visibility;
	WindowData::BlindType blindType = m_pServoModel->getBlindMetadataObject().blindType;
	uint32_t nTempEncoderValue = (m_pServoModel->getCalibrateMetadataObject().maxEncoderCounter * nOpenPercent) / 100;

	if(blindType == WindowData::BlindType::FullBlackout)
	{
		m_nTargetEncoderValue = nTempEncoderValue;
		return true;
	}
	else if(blindType == WindowData::BlindType::DayNight)
	{
		uint32_t nNearestEncoderValue;
		if(vis == WindowData::Visibility::Day)
		{

		}
		else if(vis == WindowData::Visibility::Night)
		{

		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	//uint64_t tempPercent = percent *

	//m_servoModel.setOpenPercent(percent);
	////	uint64_t temp = (percent / 100);
	////	//if(m_servoModel.getBlindType() == WindowData::BlindType::FullBlackout)
	////	if(m_servoModel.getBlindMetadataObject().blindType == WindowData::BlindType::FullBlackout)
	////	{
	////		m_setEncoderCounter = m_maxEncoderCounter * temp;
	////		return;
	////	}
	////	else
	////	{
	////		//Here Day/Night alghoritm becomes
	////		uint64_t setVal = m_maxEncoderCounter * temp;
	////		if(setVal == 0)
	////		{
	////			m_setEncoderCounter =0;
	////			return;
	////		}
	////		else
	////		{
	////			//Check Type
	////			uint8_t startValue;
	////			if(vis == WindowData::Visibility::Day)
	////				startValue = 2;
	////			else if(vis == WindowData::Visibility::Night)
	////				startValue = 1;
	////			else
	////			{
	////				m_setEncoderCounter = 0;
	////				return;
	////			}
	////
	////			//Prepare temp value
	////			uint64_t tempVal = 0;
	////			for(uint8_t i = startValue; i <= m_maxEncoderCounter; i += 2)
	////			{
	////				uint64_t realVal = i * m_singleEncoderStep;
	////				uint64_t diffVal = abs(realVal - setVal);
	////				if(diffVal < tempVal)
	////					break;
	////				else
	////					tempVal = diffVal;
	////			}
	////			m_setEncoderCounter = tempVal;
	////		}
	////	}
}


void CServoController::event()
{
	//Chekc if servo is initialized
	if(!checkIfIsInitialized())
	{
		return;	//Do nothing
	}

	if(isCalibrationActive())
	{
		processCalibrate();
		processManualDrive();
	}
	else
	{

	}

	//Current guard
	currentGuard();
}


