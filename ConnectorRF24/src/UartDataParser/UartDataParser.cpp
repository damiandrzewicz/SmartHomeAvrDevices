/*
 * UartDataParser.cpp
 *
 *  Created on: Sep 3, 2018
 *      Author: damian
 */

#include <string.h>
#include "UartDataParser.h"

#include "../uart/uart.h"

const char *const OpNameText[] PROGMEM = {
		"SDTD",		//SendDataToDevice
		"SCA",		//SetConnectorAddress
		"GCA",		//GetConnectorAddress
		"PUC",		//PowerUpConnector,
		"PDC",		//PowerDownConnector,
		"no"		//NotSupported
};

CUartDataParser::CUartDataParser() {
	// TODO Auto-generated constructor stub

}

CUartDataParser::~CUartDataParser() {
	// TODO Auto-generated destructor stub
}

bool CUartDataParser::parse(char *pData)
{
	//CUart::getInstance()->puts("here2:D\r\n");
	if(!checkTokenParser())
			return false;
	//CUart::getInstance()->puts("here1:D\r\n");
	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(pData, '@') != TokenParseResult::Ok)
	{
		return false;
	}
	//CUart::getInstance()->puts("here:D\r\n");

	//Parse
	if(m_pTokenParser->parseData(pData, getAdditionalText(AdditionalTexts::At)) != TokenParseResult::Ok)
	{
		return false;
	}


	//Get operation name
	char *cc = m_pTokenParser->getNextToken();
	//CUart::getInstance()->puts(cc);
	OperationName opName = parseOperationName(cc);

	if(opName == OperationName::NotSupported)
		return false;


//		CUart::getInstance()->puts("\r\n");
//		CUart::getInstance()->putint(static_cast<uint8_t>(opName), 10);
//		CUart::getInstance()->puts("\r\n");
//		CUart::getInstance()->puts(getOperationNameText(opName));
//		CUart::getInstance()->puts("here1\r\n");

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

bool CUartDataParser::createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult)
{
	if(!pResult)
			return false;
		//Prepare output message
		strcpy(pResult, getAdditionalText(AdditionalTexts::At));
		strcat(pResult, getOperationNameText(opName));
		strcat(pResult, getAdditionalText(AdditionalTexts::At));
		strcat(pResult, getOperationDirectonText(opDir));
		strcat(pResult, getAdditionalText(AdditionalTexts::At));
		strcat(pResult, pContext);
		strcat(pResult, getAdditionalText(AdditionalTexts::At));

		return true;
}

CUartDataParser::OperationName CUartDataParser::getOperationName()
{
	return static_cast<OperationName>(CParserInterface::getOperationNameAsUint());
}

void CUartDataParser::setOperationName(OperationName op)
{
	CParserInterface::setOperationName(static_cast<uint8_t>(op));
}

char *CUartDataParser::getOperationNameText(OperationName op)
{
	return (char*)pgm_read_word( &OpNameText[static_cast<uint8_t>(op)] );
}

CUartDataParser::OperationName CUartDataParser::parseOperationName(char *pOperationName)
{
	if(!strcmp(pOperationName, getOperationNameText(OperationName::SendDataToDevice)))
		return OperationName::SendDataToDevice;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetConnectorAddress)))
		return OperationName::GetConnectorAddress;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SetConnectorAddress)))
		return OperationName::SetConnectorAddress;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::PowerUpConnector)))
		return OperationName::PowerUpConnector;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::PowerDownConnector)))
		return OperationName::PowerDownConnector;
	else /*if(!strcmp(pOperationName, getOperationNameText(OperationName::NotSupported)))*/
		return OperationName::NotSupported;
}

bool CUartDataParser::createErrorMsg(uint8_t err, char *pResult)
{
	return CParserInterface::createErrorMsg(err, AdditionalTexts::At, pResult);
}

