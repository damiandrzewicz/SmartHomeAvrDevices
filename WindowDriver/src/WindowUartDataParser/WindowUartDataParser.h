/*
 * ModelDataParser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#pragma once

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "ParserInterface/CParserInterface.h"
#include "../servo/servo.h"
#include "../WindowUartDataParser/ModelDataContainer.h"

class CWindowUartDataParser : public CParserInterface {
public:
	enum class OperationName
	{
		SetBlindType,
		GetBlindType,
		SetState,
		GetState,
		Calibrate,
		NotSupported
	};

	CWindowUartDataParser();
	~CWindowUartDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult = nullptr);

	virtual bool createErrorMsg(Error err, char *pResult) override;

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

	bool parseBlindNo(uint8_t &iBlindNo);

	bool parseGetBlindType(SBlindType &refBlindType);
	bool parseSetBlindType(SBlindType &refBlindType);

	bool parseSetCalibrate(SBlindCalibrate &refBlindCalibrate);

	bool parseGetBlindState(SBlindState &refBlindState);
	bool parseSetBlindState(SBlindState &refBlindState);

	//bool parseSetBlindType(SBlindType &refBlindState);
	//bool parseSetBlindState()

	bool createGetBlindTypeContext(SBlindType &refBlindType, char *pResult = nullptr);
	bool createGetBlindStateContext(SBlindState &refBlindState, char *pResult = nullptr);

protected:

	void setOperationName(OperationName op);
	OperationName parseOperationName(char *pOperationName);

	char m_modelContextArr[15];
};

