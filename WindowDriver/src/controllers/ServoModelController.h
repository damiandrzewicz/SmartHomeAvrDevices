/*
 * ServoModelController.h
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#pragma once

#include "TokenParser/TokenParser.h"
#include "../UartDataParser/UartDataParser.h"
#include "../ModelDataParser/ModelDataParser.h"
#include "../servo/servo.h"

class CServoModelController {
public:
	CServoModelController();
	~CServoModelController();

	void uartDataReady(char *pData);
	char *getData();

	void setServoModel1(ServoModel *state);
	void setServoModel2(ServoModel *state);

	void loopEvent();

protected:
	void clearNotofications();

private:
	CTokenParser m_tokenParser;
	CUartDataParser m_uartParser;
	CModelDataParser m_modelParser;

	char *m_pData;
	char m_cMessage[50];

	bool m_bUartDataReady = false;

	bool m_bTimeout = false;
	unsigned long m_lTime = 0;
	uint16_t m_nTimeoutValue = 300;		//300 ms

	ServoModel *m_servoModel1 = nullptr;
	ServoModel *m_servoModel2 = nullptr;
};

