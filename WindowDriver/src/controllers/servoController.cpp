/*
 * servoController.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include "servoController.h"

uint8_t CServoController::m_sTimerHandle = 0;

CServoController::CServoController() {

	m_sTimerHandle++;
}

CServoController::~CServoController() {
	// TODO Auto-generated destructor stub
}

void CServoController::registerObjects(CServo *ptr, CQuadratureEncoder *encoder)
{
	m_servo = ptr;
	m_encoder = encoder;
}

void CServoController::event()
{

}

//Timer interface
void CServoController::incrementtimerTick()
{
	m_nTimerValue++;
}

uint8_t CServoController::getTimerHandle() const
{
	return m_sTimerHandle;
}

bool CServoController::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}

void CServoController::startTimer()
{
	resetTimerValue();
	CTimer2::getInstance()->Enable(m_sTimerHandle);
}

void CServoController::stopTimer()
{
	CTimer2::getInstance()->Disable(m_sTimerHandle);
}

void CServoController::resetTimerValue()
{
	m_nTimerValue = 0;
}
