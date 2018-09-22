/*
 * ModelDataParser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */
#include <string.h>
#include "ModelDataParser.h"

#include "../uart/uart.h"
#include "TokenParser/TokenParser.h"

const char *const OpNameText[] PROGMEM = {
		"SBT",			//SetBlindType,
		"GBT",			//GetBlindType,
		"SS",			//SetState,
		"GS",			//GetState,
		"C",			//Calibrate,
		"no",			//NotSupported
};

CModelDataParser::CModelDataParser() {
	// TODO Auto-generated constructor stub

}

CModelDataParser::~CModelDataParser() {
	// TODO Auto-generated destructor stub
}

bool CModelDataParser::parse(char *pData)
{
	if(!checkTokenParser())
			return false;

	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(pData, '!') != TokenParseResult::Ok)
	{
		return false;
	}

	//Parse
	if(m_pTokenParser->parseData(pData, getAdditionalText(AdditionalTexts::ExclMark)) != TokenParseResult::Ok)
	{
		return false;
	}

	//Get operation name
	OperationName opName = parseOperationName(m_pTokenParser->getNextToken());
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
	else
		return false;

	return true;
}

bool CModelDataParser::createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult)
{
	if(!pResult)
		return false;

	//Prepare output message
	strcpy(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	strcat(pResult, getOperationNameText(opName));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	strcat(pResult, getOperationDirectonText(opDir));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	//strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, pContext);
	//strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));

	return true;
}

CModelDataParser::OperationName CModelDataParser::getOperationName()
{
	return static_cast<OperationName>(CParserInterface::getOperationNameAsUint());
}

void CModelDataParser::setOperationName(OperationName op)
{
	CParserInterface::setOperationName(static_cast<uint8_t>(op));
}

char *CModelDataParser::getOperationNameText(OperationName op)
{
	return (char*)pgm_read_word( &OpNameText[static_cast<uint8_t>(op)] );
}

CModelDataParser::OperationName CModelDataParser::parseOperationName(char *pOperationName)
{
	if(!strcmp(pOperationName, getOperationNameText(OperationName::SetBlindType)))
		return OperationName::SetBlindType;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetBlindType)))
		return OperationName::GetBlindType;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SetState)))
		return OperationName::SetState;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetState)))
		return OperationName::GetState;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::Calibrate)))
		return OperationName::Calibrate;
	else /*if(!strcmp(pOperationName, getOperationNameText(OperationName::NotSupported)))*/
		return OperationName::NotSupported;
}

bool CModelDataParser::createErrorMsg(Error err, char *pResult)
{
	return CParserInterface::createErrorMsg(err, AdditionalTexts::ExclMark, pResult);
}

bool CModelDataParser::parseBlindNo(uint8_t &nBlindNo)
{
	strcpy(m_modelContextArr, m_pContext);

	if(!checkTokenParser())
			return false;

	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(m_modelContextArr, '$') != TokenParseResult::Ok)
	{
		return false;
	}
	//Parse
	if(m_pTokenParser->parseData(m_modelContextArr, getAdditionalText(AdditionalTexts::Dollar)) != TokenParseResult::Ok)
	{
		return false;
	}

	uint8_t blindNo = atoi(m_pTokenParser->getNextToken());

	if(blindNo == 1 || blindNo == 2)
	{
		nBlindNo = blindNo;	//Save data
		return true;
	}
	else
		return false;
}

bool CModelDataParser::parseGetBlindType(SBlindType &refBlindType)
{
	if(!parseBlindNo(refBlindType.nBlindNo))
		return false;

	return true;
}


bool CModelDataParser::createGetBlindTypeContext(SBlindType &refBlindType, char *pResult)
{
	if(pResult == nullptr)
		return false;

	char cBlindType[3];
	itoa(static_cast<uint8_t>(refBlindType.blindType), cBlindType, 10);

	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cBlindType);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));

	return true;
}


bool CModelDataParser::createGetBlindStateContext(SBlindState &refBlindState, char *pResult)
{
	if(pResult == nullptr)
		return false;

	//TODO
}

