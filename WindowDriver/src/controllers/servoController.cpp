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
	//m_lastServoModel.setBlindNumber(pServoModel->getBlindNumber());
	m_nLastOpenPercentValue = m_pServoModel->getOpenPercent().getValue();

	//Init ADC
	m_adc.initADC(m_pServoData->pinADC.pin);

	//processRunServo(WindowData::Direction::Open, 100);
	//processSpeed(50);
	//m_currentDirection = WindowData::Direction::Close;

	//m_pServoModel->setOpenPercent(0);
	//m_bInitialPosition = true;
}

void CServoController::initPins()
{
	IO::Dir::Set(m_pServoData->pinM1.ddr, m_pServoData->pinM1.pin, IO::direction::out);
	IO::Dir::Set(m_pServoData->pinM2.ddr, m_pServoData->pinM2.pin, IO::direction::out);

	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::low);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::low);
}

void CServoController::processSpeed(const uint8_t &percent)
{
	if(percent > 100)
	{
		( (m_pServoData->timer1Obj)->*(m_pServoData->enablePwmFunPtr))(100);
	}
	else
	{
		( (m_pServoData->timer1Obj)->*(m_pServoData->enablePwmFunPtr))((percent * 255) / 100);
	}

}

//Operations
void CServoController::runClockwise(const uint8_t &speed)
{
	IO::Port::Set(m_pServoData->pinM1.port, m_pServoData->pinM1.pin, IO::state::high);
	IO::Port::Set(m_pServoData->pinM2.port, m_pServoData->pinM2.pin, IO::state::low);
	processSpeed(speed);
}

void CServoController::runCounterClockise(const uint8_t &speed)
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
	return (m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Idle)) ? false : true;
}


void CServoController::processCalibrate()
{
	if(m_pServoModel->getCalibrateStep() != m_lastCalibrationStep)
	{
		m_bCalibrationStepReady = true;
	}

	if(m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::ActivateCalibration)
			&& m_bCalibrationStepReady)
	{
		//CUart::getInstance()->puts("activ calibr\r\n");
		m_bCalibrationStepReady = false;
	}
	else if( (m_pServoModel->getCalibrateStep() >= static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::StartPoint)
			&& m_pServoModel->getCalibrateStep() <= static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::EndPoint))
			&& m_bCalibrationStepReady)
	{
		if( m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::StartPoint ))
		{
			//CUart::getInstance()->puts("enc=");
			///CUart::getInstance()->putll(m_pEncoder->getCounter(), 10);
			//CUart::getInstance()->puts("\r\n");

			m_pEncoder->resetCounter();

			//CUart::getInstance()->puts("enc=");
			//CUart::getInstance()->putll(m_pEncoder->getCounter(), 10);
			//CUart::getInstance()->puts("\r\n");
		}
		else
		{
			m_pServoModel->setPositionArrayValue(m_pServoModel->getCalibrateStep(), m_pEncoder->getCounter());
			//CUart::getInstance()->puts("arrVal=");
			//CUart::getInstance()->putll(m_pServoModel->getPositionArrayValue(m_pServoModel->getCalibrateStep()), 10);
			//CUart::getInstance()->puts("\r\n");
		}

		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::PointsReady)
			&& m_bCalibrationStepReady)
	{
		//TODO save array in EEPROM

		m_bCalibrationStepReady = false;
	}
	else if(m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::FullClose)
			&& m_bCalibrationStepReady)
	{
		//PORTB ^= (1 << PB7);
		//CUart::getInstance()->puts("FullClose\r\n");
		m_pServoModel->setManualDriveDirection(WindowData::Direction::Open);
		m_bCalibrationStepReady = false;

		m_pServoModel->setCalibrateStep(static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Offset));
	}
	else if(m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Offset)
			&& m_bCalibrationStepReady
			&& m_nCurrentGuardError)
	{
		//PORTB ^= (1 << PB7);

		//m_pEncoder->setCounter(m_pServoModel->getCalibrateMetadataObject().offsetEncoder);
		m_pServoModel->getCalibrateMetadataObject().offsetEncoder = m_pEncoder->getCounter();

		//if(!checkIfIsCalibrated())
		//{
		//	CUart::getInstance()->puts("noCal-setOffset\r\n");
		//	m_pServoModel->getCalibrateMetadataObject().offsetEncoder = m_pEncoder->getCounter();
		//}
		//else
		//{
		//	CUart::getInstance()->puts("Cal-setcounter\r\n");

		//}

		m_bCalibrationStepReady = false;
		m_pServoModel->setCalibrateStep(static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Finished));

		//CUart::getInstance()->puts("Offset\r\n");
		//CUart::getInstance()->puts("offsetEncoder=");
		//CUart::getInstance()->putll(m_pServoModel->getCalibrateMetadataObject().offsetEncoder, 10);
		//CUart::getInstance()->puts("\r\n");
	}
	else if(m_pServoModel->getCalibrateStep() == static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Finished)
			)
	{
		//PORTB ^= (1 << PB7);

		//CUart::getInstance()->puts("Finished\r\n");
		//CUart::getInstance()->puts("Finished calibr\r\n");
		//CUart::getInstance()->puts("offsetEncoder=");
		//CUart::getInstance()->putll(m_pServoModel->getCalibrateMetadataObject().offsetEncoder, 10);
		//CUart::getInstance()->puts("\r\n");

		//Commit eeprom here

		//Open to position 0
		m_nCurrentGuardError = false;


		//m_pServoModel->setOpenPercent(0);
		//if(!checkIfIsCalibrated())
		//{
			m_currentDirection = WindowData::Direction::Close;
		//}
		//else
		//{
			//m_currentDirection = WindowData::Direction::Open;
		//}

		//m_bInitialPosition = true;

		m_pServoModel->setCalibrateStep(static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Idle));

		if(!checkIfIsCalibrated())
		{
			m_pServoModel->getCalibrateMetadataObject().bIsCalibrated = true;
		}

		m_bCalibrationStepReady = false;

	}

	m_lastCalibrationStep = m_pServoModel->getCalibrateStep();
}

bool CServoController::isGoingToInitPositionActive()
{
	return (m_nItialPositionStep != 0) ? true : false;
}

void CServoController::processInitialPosition()
{
	if(m_nItialPositionStep == 1)
	{
		//m_pServoModel->setManualDriveDirection(WindowData::Direction::Open);
		m_currentDirection = WindowData::Direction::Open;
		m_nItialPositionStep = 2;
		//CUart::getInstance()->puts("initPos-1\r\n");
	}
	else if(m_nItialPositionStep == 2 && m_nCurrentGuardError)
	{
		m_pEncoder->setCounter(m_pServoModel->getCalibrateMetadataObject().offsetEncoder);
		m_nItialPositionStep = 3;
		//CUart::getInstance()->puts("initPos-2\r\n");
	}
	else if(m_nItialPositionStep == 3)
	{
		m_currentDirection = WindowData::Direction::Close;
		m_nItialPositionStep = 0;
		//CUart::getInstance()->puts("initPos-3\r\n");
	}
}

void CServoController::startGointToInitPosition()
{
	if( checkIfIsCalibrated() )
	{
		m_nItialPositionStep = 1;
	}
}

bool CServoController::checkIfIsCalibrated()
{
	return m_pServoModel->getCalibrateMetadataObject().bIsCalibrated;
}

uint16_t CServoController::currentMeasure()
{
	uint32_t voltage =  (m_adc.averageADC() * 25);	//mV * 10
	//return voltage;

	return ( (voltage * 100) / (m_pServoData->adcResistorValue) );
}

void CServoController::currentGuard()
{

	//current = currentMeasure();
	if(isServoRunning())
	{
		if(m_bCurrentGuardMeasureDelay)
		{
			if(!m_lTime)
			{//CUart::getInstance()->puts("timo=0\r\n");
				m_lTime = millis();

				//CUart::getInstance()->puts("guardTimeoutStart=");
				//CUart::getInstance()->putll(m_lTime, 10);
				//CUart::getInstance()->puts("\r\n");
			}
			else
			{
				//uint16_t r = (millis() - m_lTime);
				if( (millis() - m_lTime) > m_nTimeoutValue)
				{
					//CUart::getInstance()->puts("guardTimeout=\r\n");
					//CUart::getInstance()->putll(r, 10);
					//CUart::getInstance()->puts("\r\n");
					setCurrentGuardDelayInactive();
				}
			}
		}
		else
		{
			//uint16_t currentValue = currentMeasure();
			if(!m_bCurrentGuardMeasureDelay && (currentMeasure() > m_pServoData->adcMaxCurrent) )
			{
				m_bBlockModelOperations = true;
				m_nCurrentGuardError = true;
				m_currentDirection = WindowData::Direction::Stop;	//Stop servo
				//CUart::getInstance()->puts("guardStop\r\n");
				//CUart::getInstance()->puts("guardCurr=");
				//CUart::getInstance()->putll(currentValue, 10);
				//CUart::getInstance()->puts("\r\n");
			}
		}



	}
}

void CServoController::setCurrentGuardDelayActive()
{
	m_bCurrentGuardMeasureDelay = true;
}

void CServoController::setCurrentGuardDelayInactive()
{
	m_bCurrentGuardMeasureDelay = false;
	m_lTime = 0;
}

void CServoController::encoderGuard()
{
	bool bInverted = false;
//	if(m_pServoModel->getBlindMetadataObject().motorSide == WindowData::MotorSide::Left)
//	{
//		bInverted = true;
//	}

	if(m_currentDirection == WindowData::Direction::Close)
	{

		if(m_pEncoder->getCounter(bInverted) >= m_nTargetEncoderValue)
		{
			//CUart::getInstance()->puts("encGuard-close-stop\r\n");
			m_currentDirection = WindowData::Direction::Stop;
		}
	}
	else if(m_currentDirection == WindowData::Direction::Open)
	{
		if(m_pEncoder->getCounter(bInverted) < m_nTargetEncoderValue)
		{
			//CUart::getInstance()->puts("encGuard-open-stop\r\n");
			m_currentDirection = WindowData::Direction::Stop;
		}
	}
}

void CServoController::processRunServo(/*WindowData::Direction dir, uint8_t speed*/)
{
	if(m_currentDirection == m_lastDirection)
	{
		return;
	}

	if(!m_bBlockModelOperations && m_nCurrentGuardError)
	{
		if(m_currentDirection == m_lastDirectionCurrentGuard || m_currentDirection == WindowData::Direction::Stop)
			return;
		else
			m_nCurrentGuardError = false;
	}

	if(m_currentDirection != m_lastDirection)
	{
		m_lastDirection = m_currentDirection;

		if(m_currentDirection != WindowData::Direction::Stop)
			m_lastDirectionCurrentGuard = m_currentDirection;
	}
//
//	if(m_currentDirection == m_lastDirection)
//	{
//		return;
//	}
//	else
//	{
//		m_lastDirection = m_currentDirection;
//	}

	//CUart::getInstance()->puts("currDir=\r\n");
	//CUart::getInstance()->putint(static_cast<uint8_t>(m_currentDirection), 10);
	//CUart::getInstance()->puts("\r\n");

	//CUart::getInstance()->puts("lastDir=\r\n");
	//CUart::getInstance()->putint(static_cast<uint8_t>(m_lastDirection), 10);
	//CUart::getInstance()->puts("\r\n");

	if(m_currentDirection == WindowData::Direction::Open)
	{
		m_bIsRunning = true;
		runClockwise(m_nSpeed);
		setCurrentGuardDelayActive();
	}
	else if(m_currentDirection == WindowData::Direction::Close)
	{
		//CUart::getInstance()->puts("exec close\r\n");
		m_bIsRunning = true;
		runCounterClockise(m_nSpeed);
		setCurrentGuardDelayActive();
	}
	else
	{
		//CUart::getInstance()->puts("exec stop\r\n");
		m_bIsRunning = false;
		stop();
		setCurrentGuardDelayInactive();
	}

	if(m_nCurrentGuardError && m_bBlockModelOperations)
		m_bBlockModelOperations = false;
}

bool CServoController::isServoRunning()
{
	return m_bIsRunning;
}

void CServoController::processManualDrive()
{
	//if(m_bBlockMotion)
	//	return;

	if(m_bBlockModelOperations)
	{
		return;
	}

	if(m_pServoModel->getManualDriveDirection().hasChanged())
	{
		m_currentDirection = m_pServoModel->getManualDriveDirection().getValue();
		m_pServoModel->getManualDriveDirection().setHasChanged(false);
	}

	m_nSpeed = 100;
}

void CServoController::processAutoDrive()
{
	if(m_bBlockModelOperations)
	{
		return;
	}

//	static bool testV = false;
//
//	if(!testV)
//	{
//		testV = true;
//		m_nTargetEncoderValue = m_pServoModel->getPositionArrayValue(5) + m_pServoModel->getPositionArrayValue(5);
//		m_currentDirection = WindowData::Direction::Close;
//		CUart::getInstance()->puts("processAutoDriveNew - Target=");
//		CUart::getInstance()->putll(m_nTargetEncoderValue, 10);
//		CUart::getInstance()->puts("\r\n");
//	}

	if(m_pServoModel->getOpenPercent().hasChanged())
	{
		calculateTargetEncoderValue();

		CUart::getInstance()->puts("processAutoDrive - Target=");
		CUart::getInstance()->putll(m_nTargetEncoderValue, 10);
		CUart::getInstance()->puts("\r\n");

		//CUart::getInstance()->putll(m_nTargetEncoderValue, 10);


		if(m_pServoModel->getOpenPercent().getValue() > m_nLastOpenPercentValue)
		{PORTB ^= (1 << PB7);
			//CUart::getInstance()->puts("call close\r\n");
			m_currentDirection = WindowData::Direction::Close;
			m_nLastOpenPercentValue = m_pServoModel->getOpenPercent().getValue();
		}
		else if(m_pServoModel->getOpenPercent().getValue() < m_nLastOpenPercentValue)
		{PORTB ^= (1 << PB7);
			//CUart::getInstance()->puts("call open\r\n");
			m_currentDirection = WindowData::Direction::Open;
			m_nLastOpenPercentValue = m_pServoModel->getOpenPercent().getValue();
		}
		else
		{
			m_currentDirection = WindowData::Direction::Stop;
		}

		m_pServoModel->getOpenPercent().setHasChanged(false);
	}
}

bool CServoController::calculateTargetEncoderValue()
{
	uint32_t nOpenPercent = m_pServoModel->getOpenPercent().getValue();

	CUart::getInstance()->puts("Open%=");
	CUart::getInstance()->putint(nOpenPercent, 10);
	CUart::getInstance()->puts("\r\n");

	WindowData::Visibility vis = m_pServoModel->visibility;
//
	CUart::getInstance()->puts("Vis%=");
	CUart::getInstance()->putint(static_cast<uint8_t>(vis), 10);
	CUart::getInstance()->puts("\r\n");

	WindowData::BlindType blindType = m_pServoModel->getBlindMetadataObject().blindType;
	CUart::getInstance()->puts("BlindTyp%=");
	CUart::getInstance()->putint(static_cast<uint8_t>(blindType), 10);
	CUart::getInstance()->puts("\r\n");

	if(blindType == WindowData::BlindType::FullBlackout)
	{
		if(m_pServoModel->getCalibrateMetadataObject().nRegisteredSteps == 1)
		{
			m_nTargetEncoderValue = (m_pServoModel->getPositionArrayValue(1) * nOpenPercent) / 100;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(blindType == WindowData::BlindType::DayNight)
	{
		CUart::getInstance()->puts("Bt-DN\r\n");

		uint8_t nRealSteps = m_pServoModel->getCalibrateMetadataObject().nRegisteredSteps;
		uint8_t nVirtualSteps = nRealSteps;
		if(vis == WindowData::Visibility::Night)
		{
			nVirtualSteps += 1;
		}

		CUart::getInstance()->puts("nSteps=");
		CUart::getInstance()->putll(nRealSteps, 10);
		CUart::getInstance()->puts("\r\n");

		CUart::getInstance()->puts("nVirtualSteps=");
		CUart::getInstance()->putll(nVirtualSteps, 10);
		CUart::getInstance()->puts("\r\n");

		uint8_t nVirtualPos = ( (nVirtualSteps * nOpenPercent) / 100) + 0.5;

		CUart::getInstance()->puts("nVirtualPos=");
		CUart::getInstance()->putll(nVirtualPos, 10);
		CUart::getInstance()->puts("\r\n");

		if(!nVirtualPos)
		{
			m_nTargetEncoderValue = 0;
		}
		else if(vis == WindowData::Visibility::Day)
		{
			m_nTargetEncoderValue = m_pServoModel->getPositionArrayValue(nVirtualPos - 1);
		}
		else if(vis == WindowData::Visibility::Night)
		{

			if(nVirtualPos == 1)
			{
				uint16_t currentPos = m_pServoModel->getPositionArrayValue(0);
				m_nTargetEncoderValue = currentPos - (currentPos / 2);
			}
			else if( nVirtualPos <= nRealSteps)
			{
				uint16_t currentPos = m_pServoModel->getPositionArrayValue(nVirtualPos - 2);
				uint16_t nextPos = m_pServoModel->getPositionArrayValue(nVirtualPos -1);
				m_nTargetEncoderValue = currentPos + ((nextPos - currentPos) / 2);
			}
			else if(nVirtualPos == nRealSteps + 1)
			{
				uint16_t currentPos = m_pServoModel->getPositionArrayValue(nVirtualPos - 2);
				uint16_t prevPos = m_pServoModel->getPositionArrayValue(nVirtualPos -3);
				m_nTargetEncoderValue = currentPos + ((currentPos - prevPos) / 2);
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}


void CServoController::event()
{
	/*
	//Chekc if servo is initialized
	if(!checkIfIsInitialized())
	{
		return;	//Do nothing
	}

*/
	if(isCalibrationActive())
	{
		PORTB &= ~(1 << PB5);
		processCalibrate();
		processManualDrive();
	}
	else if(isGoingToInitPositionActive())
	{
		processInitialPosition();
	}
	else
	{
		PORTB |= (1 << PB5);
		encoderGuard();
		processAutoDrive();
	}

	processRunServo();
	currentGuard();
}


