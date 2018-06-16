/*
 * servoController.h
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_SERVOCONTROLLER_H_
#define SRC_CONTROLLERS_SERVOCONTROLLER_H_

#include "../servo/servo.h"

class CServoController {
public:
	CServoController();
	~CServoController();

	//Setters
	void registerServo(CServo *ptr);

	void controllerEvent();

private:
	CServo *m_servo = nullptr;
};

#endif /* SRC_CONTROLLERS_SERVOCONTROLLER_H_ */
