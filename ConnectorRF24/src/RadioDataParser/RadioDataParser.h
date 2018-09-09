/*
 * RadioDataParser.h
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#pragma once

#include <avr/pgmspace.h>
#include "TokenParser/TokenParser.h"
#include "ParserInterface/CParserInterface.h"



class CRadioDataParser : public CParserInterface {
public:
	enum class OperationName{
		PassDataByAir,
		NotSupported = 100
	};

	CRadioDataParser();
	~CRadioDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, const char *pContext, char *pResult = nullptr);

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

protected:
	void setOperationName(OperationName op);

	OperationName parseOperationName(char *pOperationName);
};

