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

struct ServoData
{
	IO::PinData pinM1;
	IO::PinData pinM2;
	void (CTimer1::*enablePwmFunPtr)(uint8_t) = nullptr;
	CTimer1 *timer1Obj;
};

class CServo {
public:

	enum Direction{ Open, Close, Stop};
	enum Visibility{ Day, Night, None };
	enum BlindType{ DayNight, FullBlackout };

	CServo(ServoData data);
	~CServo();


	//Setters
	void setEnablePwmFunPtr(void (CTimer1::*enablePwmFunPtr)(uint8_t));
	void setMaxEncoderCounter(uint64_t value);
	void setSingleEncoderStep(uint8_t value);
	void setBlindType(BlindType blindType);

	void setDirection(Direction dir);
	void setOpenPercent(uint8_t value);
	void setSpeedPercent(uint8_t value);
	void setAutoDrive(bool value);
	void setVisibility(Visibility vis);
	void setWindowClosed(bool value);

	//Getters
	const int64_t &getMaxEncoderCounter() const;
	BlindType getblindType();

	Direction getDirection();
	uint8_t getOpenPercent();
	uint8_t getSpeedPercent();
	bool isAutoDrive();
	Visibility getVisibility();
	bool isWindowClosed();

	//Operations
	void processSpeed(uint8_t percent);
	void processPercentOfOpen(uint8_t percent, Visibility vis = Visibility::None);
	void processRunServo(Direction dir, uint8_t speed = 255);

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

	BlindType m_blindType = BlindType::FullBlackout;

	Direction m_dir = Direction::Stop;
	uint8_t m_nOpenPercent = 0;
	uint8_t m_nSpeedPercent = 100;
	bool m_bAutoDrive = true;
	Visibility m_visibility = Visibility::None;
	bool m_bIsWindowClosed = false;
};

#endif /* SRC_SERVO_SERVO_H_ */
