/*
 * ParserInterface.h
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#pragma once

#include <string.h>
#include <avr/pgmspace.h>
#include "../TokenParser/TokenParser.h"

const char *const OpDirectionText[] PROGMEM = {
		"req",
		"res",
		"no"
};

const char *const AdditionalLettersText[] PROGMEM = {
		"@"
};

class CParserInterface
{
public:

	enum class OperationDirection{Request, Response, NotSupported};
	enum class AdditionalLetter{ At };

	CParserInterface(){}
	virtual ~CParserInterface(){}

	virtual bool parse(char *pData) = 0;

	void registerTokenParser(CTokenParser *pTokenParser){m_pTokenParser = pTokenParser;}
	bool checkTokenParser(){return (m_pTokenParser != nullptr) ? true : false;}

	OperationDirection getOperationDirection(){return m_operationDirection;}
	char *getOperationDirectonText(OperationDirection op){
		if(op == OperationDirection::Request)
			return (char*)pgm_read_word( &OpDirectionText[0] );
		else if(op == OperationDirection::Request)
			return (char*)pgm_read_word( &OpDirectionText[1] );
		else
			return (char*)pgm_read_word( &OpDirectionText[2] );
	}

	const char *getContext(){return m_pContext;}

	char *getAdditionalLetter(AdditionalLetter adt){
		return (char*)pgm_read_word( &AdditionalLettersText[static_cast<uint8_t>(AdditionalLetter::At)] );
	}

protected:
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

private:
	OperationDirection m_operationDirection;
	uint8_t m_operationName = 100;
};
