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



	void event();

private:
	void initPins();

	void processSpeed(uint8_t percent);
	void runClockwise(uint8_t speed);
	void runCounterClockise(uint8_t speed);
	void stop();
	void processRunServo(WindowData::Direction dir, uint8_t speed);
	bool calculateTargetEncoderValue();

	void processCalibrate();
	void processManualDrive();
	//void processFullBlackoutCalibration();
	//void processDayNightCalibration();
	//void processOffsetCalibration();

	bool checkIfIsInitialized();
	bool isCalibrationActive();

	uint16_t currentMeasure();
	void currentGuard();
	void encoderGuard();

	uint32_t findClosestValue(uint32_t nMaxRange, uint32_t nSingleStep, uint32_t nSetValue = 0, bool bUseSetValue = false);


private:
	ServoData *m_pServoData = nullptr;
	CQuadratureEncoder *m_pEncoder = nullptr;
	CServoModel *m_pServoModel;
	CADC m_adc;


	int32_t m_nTargetEncoderValue;
	uint8_t m_nLastOpenPercentValue = 0;

	uint16_t m_sTimeout = 100;			//Timeout 500ms

	bool m_bCalibrationStepReady = false;
	bool m_nCurrentError = false;

};

#endif /* SRC_CONTROLLERS_SERVOCONTROLLER_H_ */
