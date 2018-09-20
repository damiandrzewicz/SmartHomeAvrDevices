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


class CBaseController {
public:
	CBaseController();
	virtual ~CBaseController();

	//Notofications
	void radioDataReady(char *pData);
	void uartDataReady(char *pData);
	char *getData();

	virtual void eventLoop() = 0;

protected:
	void clearNotofications();

protected:
	CTokenParser m_tokenParser;
	CRadioDataParser m_radioParser;
	CUartDataParser m_uartParser;
	char *m_pData;
	char m_cMessage[50];


	bool m_bRadioDataReady = false;
	bool m_bUartDataReady = false;

	bool m_bTimeout = false;
	unsigned long m_lTime = 0;
	uint16_t m_nTimeoutValue = 300;		//300 ms
};


