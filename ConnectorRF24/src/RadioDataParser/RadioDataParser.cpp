/*
 * RadioDataParser.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#include "../RadioDataParser/RadioDataParser.h"

#include <avr/pgmspace.h>
#include "../uart/uart.h"

const char *const OpNameText[] PROGMEM = {
		"PDBA",	//Pass Data By Air
		"no"
};


CRadioDataParser::CRadioDataParser() {
	// TODO Auto-generated constructor stub

}

CRadioDataParser::~CRadioDataParser() {
	// TODO Auto-generated destructor stub
}

bool CRadioDataParser::parse(char *pData)
{
	if(!checkTokenParser())
		return false;

	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(pData, '#') != TokenParseResult::Ok)
	{
		return false;
	}

	//Parse
	if(m_pTokenParser->parseData(pData, getAdditionalText(AdditionalTexts::Hash)) != TokenParseResult::Ok)
	{
		return false;
	}

	//Get operation name
	char *cc = m_pTokenParser->getNextToken();
	OperationName opName = parseOperationName(cc);
	if(opName == OperationName::NotSupported)
		return false;

	setOperationName(opName);

	//Get operation direction
	OperationDirection opDir = CParserInterface::parseOperationDirection(m_pTokenParser->getNextToken());
	if(opDir == OperationDirection::NotSupported)
		return false;

	setOperationDirection(opDir);

	char *pContext = m_pTokenParser->getNextToken();
	if(pContext != nullptr && strcmp(pContext, PSTR("\0")))
			m_pContext = pContext;

	return true;

}

CRadioDataParser::OperationName CRadioDataParser::parseOperationName(char *pOperationName)
{
	if(!strcmp(pOperationName, getOperationNameText(OperationName::PassDataByAir)))
		return OperationName::PassDataByAir;
	else
		return OperationName::NotSupported;
}

char *CRadioDataParser::getOperationNameText(OperationName op)
{
	return (char*)pgm_read_word( &OpNameText[static_cast<uint8_t>(op)] );
}

CRadioDataParser::OperationName CRadioDataParser::getOperationName()
{
	return static_cast<OperationName>(CParserInterface::getOperationNameAsUint());
}

void CRadioDataParser::setOperationName(OperationName op)
{
	CParserInterface::setOperationName(static_cast<uint8_t>(op));
}

bool CRadioDataParser::createMessage(OperationName opName, OperationDirection opDir, const char *pContext, char *pResult)
{
	if(!pResult)
		return false;
	//Prepare output message
	strcpy(pResult, getAdditionalText(AdditionalTexts::Hash));
	strcat(pResult, getOperationNameText(opName));
	strcat(pResult, getAdditionalText(AdditionalTexts::Hash));
	strcat(pResult, getOperationDirectonText(opDir));
	strcat(pResult, getAdditionalText(AdditionalTexts::Hash));
	strcat(pResult, pContext);
	strcat(pResult, getAdditionalText(AdditionalTexts::Hash));

	return true;
}

bool CRadioDataParser::createErrorMsg(uint8_t err, char *pResult)
{
	return CParserInterface::createErrorMsg(err, AdditionalTexts::Hash, pResult);
}


