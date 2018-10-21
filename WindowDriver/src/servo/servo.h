/*
 * servo.h
 *
 *  Created on: Jun 13, 2018
 *      Author: damian
 */

#ifndef SRC_SERVO_SERVO_H_
#define SRC_SERVO_SERVO_H_

#include "../io/io.h"
#include "../timer/timer.h"
#include "ServoModel.h"

struct ServoData
{
	IO::PinData pinM1;
	IO::PinData pinM2;
	void (CTimer1::*enablePwmFunPtr)(uint8_t) = nullptr;
	CTimer1 *timer1Obj;
};

class CServo {
public:

	CServo(ServoData data, uint8_t nBlindNo);
	~CServo();

	//Setters
	void setEnablePwmFunPtr(void (CTimer1::*enablePwmFunPtr)(uint8_t));
	void setMaxEncoderCounter(uint64_t value);
	void setSingleEncoderStep(uint8_t value);


	//Getters
	const int64_t &getMaxEncoderCounter() const;


	CServoModel *getServoModel();

	//Operations
	void processSpeed(uint8_t percent);
	void processPercentOfOpen(uint8_t percent, WindowData::Visibility vis = WindowData::Visibility::None);
	void processRunServo(WindowData::Direction dir, uint8_t speed = 255);

	//Operations
private:
	void runClockwise(uint8_t speed);
	void runCounterClockise(uint8_t speed);
	void stop();

	//Members
private:
	IO::PinData m_pinM1;
	IO::PinData m_pinM2;

	void (CTimer1::*enablePwmFunPtr)(uint8_t) = nullptr;
	CTimer1 *m_timer1Obj = nullptr;

	int64_t m_maxEncoderCounter = 0;
	uint8_t m_singleEncoderStep = 0;
	uint64_t m_setEncoderCounter = 0;

	CServoModel m_servoModel;
};



#endif /* SRC_SERVO_SERVO_H_ */
