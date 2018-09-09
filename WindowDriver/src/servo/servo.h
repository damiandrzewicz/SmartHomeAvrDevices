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

class ServoEnum
{
public:
	enum class Direction{ Open, Close, Stop};
	enum class Visibility{ Day, Night, None };
	enum class BlindType{ DayNight, FullBlackout };
};

class ServoModel
{
public:
	ServoModel(){}
	~ServoModel(){}

	ServoEnum::Direction getDirection(){ return m_dir; }
	uint8_t getOpenPercent(){ return m_nOpenPercent; }
	uint8_t getSpeedPercent(){ return m_nSpeedPercent; }
	bool isAutoDrive(){ return m_bAutoDrive; }
	ServoEnum::Visibility getVisibility(){ return m_visibility; }
	ServoEnum::BlindType getBlindType(){ return m_blindType; }
	bool isWindowClosed(){ return m_bIsWindowClosed; }
	uint8_t getCalibrationStep(){ return m_nCalibrateStep; }
	bool isCalibrating(){ return m_nCalibrateStep; }

	void setDirection(ServoEnum::Direction dir){ m_dir = dir; }
	void setOpenPercent(uint8_t value){ m_nOpenPercent = value; }
	void setSpeedPercent(uint8_t value){ m_nSpeedPercent = value; }
	void setAutoDrive(bool value){ m_bAutoDrive = value; }
	void setVisibility(ServoEnum::Visibility vis){ m_visibility = vis; }
	void setBlindType(ServoEnum::BlindType blindType){ m_blindType = blindType; }
	void setWindowClosed(bool value){ m_bIsWindowClosed = value; }
	void setCalibrationStep(uint8_t value){ m_nCalibrateStep = value; }

private:
	ServoEnum::Direction m_dir = ServoEnum::Direction::Stop;
	uint8_t m_nOpenPercent = 0;
	uint8_t m_nSpeedPercent = 100;
	bool m_bAutoDrive = true;
	ServoEnum::Visibility m_visibility = ServoEnum::Visibility::None;
	ServoEnum::BlindType m_blindType = ServoEnum::BlindType::FullBlackout;
	bool m_bIsWindowClosed = false;
	uint8_t m_nCalibrateStep = 0;		//0 - disabled, 1 - ..., 2 - ...
};

struct ServoData
{
	IO::PinData pinM1;
	IO::PinData pinM2;
	void (CTimer1::*enablePwmFunPtr)(uint8_t) = nullptr;
	CTimer1 *timer1Obj;
};



class CServo {
public:

	CServo(ServoData data);
	~CServo();

	//Setters
	void setEnablePwmFunPtr(void (CTimer1::*enablePwmFunPtr)(uint8_t));
	void setMaxEncoderCounter(uint64_t value);
	void setSingleEncoderStep(uint8_t value);


	//Getters
	const int64_t &getMaxEncoderCounter() const;


	ServoModel *getServoModel();

	//Operations
	void processSpeed(uint8_t percent);
	void processPercentOfOpen(uint8_t percent, ServoEnum::Visibility vis = ServoEnum::Visibility::None);
	void processRunServo(ServoEnum::Direction dir, uint8_t speed = 255);

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



	ServoModel m_servoModel;
};



#endif /* SRC_SERVO_SERVO_H_ */
