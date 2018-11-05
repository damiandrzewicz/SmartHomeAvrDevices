/*
 * BaseController.h
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#pragma once

#include "TokenParser/TokenParser.h"
#include "../RadioDataParser/RadioDataParser.h"
#include "../UartDataParser/UartDataParser.h"
#include "../RF24/RF24.h"


class CBaseController {
public:
	CBaseController();
	virtual ~CBaseController();

	//Notofications
	void radioDataReady(char *pData);
	void uartDataReady(char *pData);
	char *getData();

	virtual void eventLoop() = 0;

	bool parseUartMessageWrapper(
			CUartDataParser::OperationDirection expectedDir,
			CRadioDataParser::OperationDirection radioDir,
			CUartDataParser::OperationName &operationName);

	bool parseRadioMessageWrapper(
			CRadioDataParser::OperationDirection expectedDir,
			CUartDataParser::OperationDirection uartDir,
			CRadioDataParser::OperationName &operationName);


protected:
	void clearNotofications();

protected:
	CTokenParser m_tokenParser;
	CRadioDataParser m_radioParser;
	CUartDataParser m_uartParser;
	char *m_pData;
	char m_cMessage[50];

	CUartDataParser::OperationName m_uartOperationName;
	CRadioDataParser::OperationName m_radioOperationName;


	bool m_bRadioDataReady = false;
	bool m_bUartDataReady = false;

	bool m_bTimeout = false;
	unsigned long m_lTime = 0;
	uint16_t m_nTimeoutValue = 300;		//300 ms
};


