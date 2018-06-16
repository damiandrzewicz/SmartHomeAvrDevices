/*
 * servoController.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include "servoController.h"

CServoController::CServoController() {
	// TODO Auto-generated constructor stub

}

CServoController::~CServoController() {
	// TODO Auto-generated destructor stub
}

//Setters
void CServoController::registerServo(CServo *ptr)
{
	m_servo = ptr;
}

//Getters

void controllerEvent()
{

}

