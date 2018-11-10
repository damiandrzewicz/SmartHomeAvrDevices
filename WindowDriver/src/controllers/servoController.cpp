/*
 * servoController.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include "servoController.h"
#include "../utils/utils.h"

CServoController::CServoController() {

	//m_sTimerHandle++;
}

CServoController::~CServoController() {
	// TODO Auto-generated destructor stub
}

void CServoController::registerObjects(CServo *ptr, CQuadratureEncoder *encoder)
{
	m_servo = ptr;
	m_encoder = encoder;
}




void CServoController::initModelFromEeprom(uint8_t nBlindNo)
{
	//TEEPROMMetadata data = readMetadataFromEeprom(nBlindNo);
	//m_servoModelArr[0]
}

void CServoController::event()
{
	//m_servo->processRunServo(WindowData::Direction::Stop, 100);
	//if(m_servo->getServoModel()->getManualDriveDirection())
	//{
	//	PORTB ^= (1 << PB7);
	//}
	m_servo->processRunServo(m_servo->getServoModel()->getManualDriveDirection(), 100);
	//m_servo->getServoModel()->getManualDriveDirection()
}

//Timer interface
//void CServoController::incrementtimerTick()
//{
//	m_nTimerValue++;
//}
//
//uint8_t CServoController::getTimerHandle() const
//{
//	return m_sTimerHandle;
//}
//
//bool CServoController::isTimeout()
//{
//	if(m_nTimerValue > (m_sTimeout / 10))
//		return true;
//	else
//		return false;
//}
//
//void CServoController::startTimer()
//{
//	resetTimerValue();
//	CTimer2::getInstance()->Enable(m_sTimerHandle);
//}
//
//void CServoController::stopTimer()
//{
//	CTimer2::getInstance()->Disable(m_sTimerHandle);
//}
//
//void CServoController::resetTimerValue()
//{
//	m_nTimerValue = 0;
//}
