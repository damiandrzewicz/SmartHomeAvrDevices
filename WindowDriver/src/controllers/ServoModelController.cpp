/*
 * ServoModelController.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#include "ServoModelController.h"

CServoModelController::CServoModelController() {
	// TODO Auto-generated constructor stub

}

CServoModelController::~CServoModelController() {
	// TODO Auto-generated destructor stub
}


void CServoModelController::uartDataReady(char *pData)
{
	clearNotofications();
	m_bUartDataReady = true;
	m_pData = pData;
}

char *CServoModelController::getData()
{
	return m_pData;
}

void CServoModelController::clearNotofications()
{
	m_bUartDataReady = false;
}

void CServoModelController::setServoModel1(ServoModel *state)
{
	m_servoModel1 = state;
}

void CServoModelController::setServoModel2(ServoModel *state)
{
	m_servoModel2 = state;
}

void CServoModelController::loopEvent()
{

}

