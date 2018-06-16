/*
 * servoController.h
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_SERVOCONTROLLER_H_
#define SRC_CONTROLLERS_SERVOCONTROLLER_H_

#include "../servo/servo.h"
#include "../encoders/quadratureEncoder.h"

class CServoController {
public:
	CServoController(CServo *ptr, CQuadratureEncoder *encoder);
	~CServoController();

	//Setters
	//void registerServo();

	void event();

private:
	CServo *m_servo = nullptr;
	CQuadratureEncoder *m_encoder = nullptr;
};

#endif /* SRC_CONTROLLERS_SERVOCONTROLLER_H_ */
