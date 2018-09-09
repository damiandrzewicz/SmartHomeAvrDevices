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
		"SH",		//SayHello
		"SCA",		//SetConnectorAddress
		"GCA",		//GetConnectorAddress
		"RC",		//RestartConnector
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
	if(!checkTokenParser())
			return false;

		//Check trimming chars
		m_pTokenParser->checkTrimmingChars(pData, '@');

		//Parse
		uint8_t res = static_cast<uint8_t>(m_pTokenParser->parseData(pData, "@"));

		//Get operation name
		char *cc = m_pTokenParser->getNextToken();
		//CUart::getInstance()->puts(cc);
		OperationName opName = parseOperationName(cc);
		if(opName != OperationName::NotSupported)
			setOperationName(opName);

		//Get operation direction
		OperationDirection opDir = CParserInterface::parseOperationDirection(m_pTokenParser->getNextToken());
		if(opDir != OperationDirection::NotSupported)
			setOperationDirection(opDir);

		char *pContext = m_pTokenParser->getNextToken();
		if(pContext != nullptr && strcmp(pContext, PSTR("\0")))
				m_pContext = pContext;

		return true;
}

bool CUartDataParser::createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult)
{
	if(!pResult)
			return false;
		//Prepare output message
		strcpy(pResult, PSTR("@"));
		strcat(pResult, getOperationNameText(opName));
		strcat(pResult, PSTR("@"));
		strcat(pResult, getOperationDirectonText(opDir));
		strcat(pResult, PSTR("@"));
		strcat(pResult, pContext);
		strcat(pResult, PSTR("@"));

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
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SayHello)))
		return OperationName::SayHello;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetConnectorAddress)))
		return OperationName::GetConnectorAddress;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::RestartConnector)))
		return OperationName::RestartConnector;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SetConnectorAddress)))
		return OperationName::SetConnectorAddress;
	else /*if(!strcmp(pOperationName, getOperationNameText(OperationName::NotSupported)))*/
		return OperationName::NotSupported;
}

