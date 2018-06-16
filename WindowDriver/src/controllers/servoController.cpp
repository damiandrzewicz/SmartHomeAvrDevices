/*
 * servoController.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#include "servoController.h"

CServoController::CServoController(CServo *ptr, CQuadratureEncoder *encoder) {
	m_servo = ptr;
	m_encoder = encoder;
}

CServoController::~CServoController() {
	// TODO Auto-generated destructor stub
}


void CServoController::event()
{

}

