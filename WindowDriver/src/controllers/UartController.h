/*
 * ServoModelController.h
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#pragma once

#include "TokenParser/TokenParser.h"

#include "../ConnectorUartDataParser/ConnectorUartDataParser.h"
#include "../servo/servo.h"
#include "../WindowUartDataParser/WindowUartDataParser.h"

class CUartController {
public:
	CUartController();
	~CUartController();

	void uartDataReady(char *pData);
	char *getData();

	void setServoModel1(ServoModel *state);
	void setServoModel2(ServoModel *state);

	void loopEvent();

protected:
	void clearNotofications();

	bool readBlindType(SBlindType &refBlindType);
	bool processBlindType(SBlindType &refBlindType);

	bool readBlindState(SBlindState &refBlindState);
	bool processBlindState(SBlindState &refBlindState);

	bool processSetCalibrate(SBlindCalibrate &refBlindCalibrate);

	bool processModelMessage();
	bool parseGetBlindType();

private:
	CTokenParser m_tokenParser;
	CConnectorUartDataParser m_connectorUartParser;
	CWindowUartDataParser m_windowUartParser;

	char *m_pData;
	char m_cUartMessage[50];
	char m_cModelMessage[20];

	bool m_bUartDataReady = false;

	bool m_bTimeout = false;
	unsigned long m_lTime = 0;
	uint16_t m_nTimeoutValue = 300;		//300 ms

	bool m_bWaitingForResponse = false;
	bool m_bProcessResponseStep = false;

	ServoModel *m_servoModelArr[2];
};

