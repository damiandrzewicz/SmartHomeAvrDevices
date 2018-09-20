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

class CModelDataParser : public CParserInterface {
public:
	enum class OperationName
	{
		SayHello,
		SetBlindType,
		GetBlindType,
		SetState,
		GetState,
		Calibrate,
		NotSupported
	};

	CModelDataParser();
	~CModelDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult = nullptr);

	virtual bool createErrorMsg(Error err, char *pResult) override;

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

protected:
	void setOperationName(OperationName op);

	OperationName parseOperationName(char *pOperationName);
};

