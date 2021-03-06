/*
 * ParserInterface.h
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#pragma once

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../TokenParser/TokenParser.h"

const char *const OpDirectionText[] PROGMEM = {
		"req",
		"res",
		"no"
};

const char *const AdditionalText[] PROGMEM = {
		"@",
		"#",
		"!",
		"$",	//For Params
		"$ok$",
		"ept",	//Empty context
		"ERR",

};

class CParserInterface
{
public:

	enum class OperationDirection{Request, Response, NotSupported};
	enum class AdditionalTexts{ At, Hash, ExclMark, Dollar, OkResponse, EmptyContext, Error };

	enum class Error {
		Timeout,
		ParserError,
		WrongOperationDirection,
		WrongOperationName,
		ProcessingError,
		END_OF_ERROR
	};

	CParserInterface(){}
	virtual ~CParserInterface(){}

	virtual bool parse(char *pData) = 0;

	virtual bool createErrorMsg(uint8_t errNo, char *pResult) = 0;

	void registerTokenParser(CTokenParser *pTokenParser){m_pTokenParser = pTokenParser;}
	bool checkTokenParser(){return (m_pTokenParser != nullptr) ? true : false;}

	OperationDirection getOperationDirection(){return m_operationDirection;}
	char *getOperationDirectonText(OperationDirection op){
		return (char*)pgm_read_word( &OpDirectionText[static_cast<uint8_t>(op)] );
	}

	char *getContext(){return m_pContext;}

	char *getAdditionalText(AdditionalTexts adt){
		return (char*)pgm_read_word( &AdditionalText[static_cast<uint8_t>(adt)] );
	}

protected:
	bool createErrorMsg(uint8_t errNo, AdditionalTexts ltr, char *pResult)
	{
		if(!pResult)
			return false;
		//Prepare output message
		char temp[3];
		strcpy(pResult, getAdditionalText(ltr));
		strcat(pResult, getAdditionalText(AdditionalTexts::Error));
		strcat(pResult, getAdditionalText(ltr));
		strcat(pResult, itoa(errNo, temp, 10));
		strcat(pResult, getAdditionalText(ltr));

		return true;
	}

	void setOperationDirection(OperationDirection op){m_operationDirection = op;}

	virtual OperationDirection parseOperationDirection(char *poperationDirection){
		if(!strcmp(poperationDirection, getOperationDirectonText(OperationDirection::Request)))
			return OperationDirection::Request;
		else if(!strcmp(poperationDirection, getOperationDirectonText(OperationDirection::Response)))
			return OperationDirection::Response;
		else
			return OperationDirection::NotSupported;
	}

	//These operations should be hidden by subclass
	void setOperationName(uint8_t value){m_operationName = value;}
	uint8_t getOperationNameAsUint(){return m_operationName;}

protected:
	char *m_pContext = nullptr;
	CTokenParser *m_pTokenParser = nullptr;
	//CTokenParser m_pTokenParser;

private:
	OperationDirection m_operationDirection;
	uint8_t m_operationName = 100;
};
