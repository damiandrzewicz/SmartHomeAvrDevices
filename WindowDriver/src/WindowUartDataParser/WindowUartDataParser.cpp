/*
 * ModelDataParser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */
#include <string.h>
#include "../uart/uart.h"
#include "TokenParser/TokenParser.h"
#include "WindowUartDataParser.h"

const char *const OpNameText[] PROGMEM = {
		"SBT",			//SetBlindType,
		"GBT",			//GetBlindType,
		"SS",			//SetState,
		"GS",			//GetState,
		"SC",			//Calibrate,
		"no",			//NotSupported
};

CWindowUartDataParser::CWindowUartDataParser() {
	// TODO Auto-generated constructor stub

}

CWindowUartDataParser::~CWindowUartDataParser() {
	// TODO Auto-generated destructor stub
}

bool CWindowUartDataParser::parse(char *pData)
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

bool CWindowUartDataParser::createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult)
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

CWindowUartDataParser::OperationName CWindowUartDataParser::getOperationName()
{
	return static_cast<OperationName>(CParserInterface::getOperationNameAsUint());
}

void CWindowUartDataParser::setOperationName(OperationName op)
{
	CParserInterface::setOperationName(static_cast<uint8_t>(op));
}

char *CWindowUartDataParser::getOperationNameText(OperationName op)
{
	return (char*)pgm_read_word( &OpNameText[static_cast<uint8_t>(op)] );
}

CWindowUartDataParser::OperationName CWindowUartDataParser::parseOperationName(char *pOperationName)
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

bool CWindowUartDataParser::createErrorMsg(Error err, char *pResult)
{
	return CParserInterface::createErrorMsg(err, AdditionalTexts::ExclMark, pResult);
}

bool CWindowUartDataParser::parseBlindNo(uint8_t &nBlindNo)
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

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;

	uint8_t blindNo = atoi(cTemp);

	if(blindNo == 1 || blindNo == 2)
	{
		nBlindNo = blindNo;	//Save data
		return true;
	}
	else
		return false;
}

bool CWindowUartDataParser::parseGetBlindType(SBlindType &refBlindType)
{
	if(!parseBlindNo(refBlindType.nBlindNo))
		return false;

	return true;
}

bool CWindowUartDataParser::parseSetBlindType(SBlindType &refBlindType)
{
	if(!parseBlindNo(refBlindType.nBlindNo))
		return false;

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;

	refBlindType.blindType = static_cast<ServoEnum::BlindType>(atoi(cTemp));

	return true;
}

bool CWindowUartDataParser::parseGetBlindState(SBlindState &refBlindState)
{
	if(!parseBlindNo(refBlindState.nBlindNo))
		return false;

	return true;
}

bool CWindowUartDataParser::parseSetBlindState(SBlindState &refBlindState)
{
	if(!parseBlindNo(refBlindState.nBlindNo))
		return false;

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.nOpenPercent = static_cast<uint8_t>(atoi(cTemp));

	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.direction = static_cast<ServoEnum::Direction>(atoi(cTemp));

	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.visibility = static_cast<ServoEnum::Visibility>(atoi(cTemp));

	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.bAutoDrive = static_cast<bool>(atoi(cTemp));

	return true;
}

bool CWindowUartDataParser::parseSetCalibrate(SBlindCalibrate &refBlindCalibrate)
{
	if(!parseBlindNo(refBlindCalibrate.nBlindNo))
		return false;

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindCalibrate.nCalibrateStep = static_cast<uint8_t>(atoi(m_pTokenParser->getNextToken()));

	return true;
}

bool CWindowUartDataParser::createGetBlindTypeContext(SBlindType &refBlindType, char *pResult)
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


bool CWindowUartDataParser::createGetBlindStateContext(SBlindState &refBlindState, char *pResult)
{
	if(pResult == nullptr)
		return false;

	char cTemp[4];

	itoa(static_cast<uint8_t>(refBlindState.nOpenPercent), cTemp, 10);
	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	itoa(static_cast<uint8_t>(refBlindState.direction), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	itoa(static_cast<uint8_t>(refBlindState.visibility), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	itoa(static_cast<uint8_t>(refBlindState.bIsWindowClosed), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));

	return true;
}



