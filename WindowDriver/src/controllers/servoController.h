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
	CServoController();
	~CServoController();

	//Setters
	void registerObjects(CServo *ptr, CQuadratureEncoder *encoder);

	//Timer interface
	//void incrementtimerTick();
	//void setTimerHandle(uint8_t value);
	//uint8_t getTimerHandle() const;
	//bool isTimeout();
	//void startTimer();
	//void stopTimer();
	//void resetTimerValue();


	void event();

private:
	CServo *m_servo = nullptr;
	CQuadratureEncoder *m_encoder = nullptr;

	//static uint8_t m_sTimerHandle;
	uint16_t m_sTimeout = 100;			//Timeout 500ms
	//volatile uint16_t m_nTimerValue = 0;


};

#endif /* SRC_CONTROLLERS_SERVOCONTROLLER_H_ */
