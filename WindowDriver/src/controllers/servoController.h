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
#include "../timer/timer.h"
#include "../servo/ServoModel.h"
#include "../ADC/ADC.h"

class CServoModel;

struct ServoData
{
	IO::PinData pinM1;
	IO::PinData pinM2;
	IO::PinData pinADC;
	uint16_t adcResistorValue;
	uint16_t adcMaxCurrent;
	void (CTimer1::*enablePwmFunPtr)(uint8_t) = nullptr;
	CTimer1 *timer1Obj;
	uint8_t nServoNumber;
};


class CServoController {
public:
	CServoController();
	~CServoController();

	//Setters
	void registerObjects(ServoData *pServoData, CQuadratureEncoder *pEncoder, CServoModel *pServoModel);

	void startGointToInitPosition();


	void event();

private:
	void initPins();

	void processSpeed(const uint8_t &percent);
	void runClockwise(const uint8_t &speed);
	void runCounterClockise(const uint8_t &speed);
	void stop();
	void processRunServo();
	bool calculateTargetEncoderValue();

	void processCalibrate();
	void processInitialPosition();
	void processManualDrive();
	void processAutoDrive();

	bool checkIfIsInitialized();
	bool checkIfIsCalibrated();

	bool isCalibrationActive();
	bool isGoingToInitPositionActive();

	uint16_t currentMeasure();
	void currentGuard();
	void encoderGuard();


	bool isServoRunning();
	void setCurrentGuardDelayActive();
	void setCurrentGuardDelayInactive();

private:
	ServoData *m_pServoData = nullptr;
	CQuadratureEncoder *m_pEncoder = nullptr;
	CServoModel *m_pServoModel;
	CADC m_adc;

	//Calibration
	uint8_t m_lastCalibrationStep = static_cast<uint8_t>(CBlindCalibrate::CalibrationStep::Idle);
	bool m_bCalibrationStepReady = false;

	//Initial position
	uint8_t m_nItialPositionStep = 0;





	int16_t m_nTargetEncoderValue = 0;
	uint8_t m_nLastOpenPercentValue = 0;

	uint16_t m_sTimeout = 100;			//Timeout 500ms



	bool m_nCurrentGuardError = false;

	//For timer
	unsigned long m_lTime = 0;
	unsigned long m_nTimeoutValue = 500;
	WindowData::Direction m_lastDirection = WindowData::Direction::Stop;
	WindowData::Direction m_currentDirection = WindowData::Direction::Stop;
	WindowData::Direction m_lastDirectionCurrentGuard = WindowData::Direction::Stop;
	uint8_t m_nSpeed = 0;


	bool m_bCurrentGuardMeasureDelay = true;
	bool m_bBlockModelOperations = false;

	bool m_bIsRunning = false;

	//bool m_bInitialPosition = false;
	//uint8_t m_nInitStep = 0;

};

#endif /* SRC_CONTROLLERS_SERVOCONTROLLER_H_ */
