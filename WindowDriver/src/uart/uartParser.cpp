/*
 * uartParser.cpp
 *
 *  Created on: Jul 8, 2018
 *      Author: damian
 */

#include "uartParser.h"
#include <string.h>
#include <stdlib.h>
#include "../settings.h"
#include "uart.h"


CUartParser::CUartParser() {
	// TODO Auto-generated constructor stub

}

CUartParser::~CUartParser() {
	// TODO Auto-generated destructor stub
}

void CUartParser::setServoModel1(ServoModel *state)
{
	m_servoModel1 = state;
}

void CUartParser::setServoModel2(ServoModel *state)
{
	m_servoModel2 = state;
}

void CUartParser::uartCallback(char *data)
{
	m_pReceivedBuffer = data;
	setCheckUartMessage(true);
}

void CUartParser::setResponseBuffer(char *data)
{
	strcpy(m_responseBuffer, data);
}

char *CUartParser::getResponseBuffer()
{
	return m_responseBuffer;
}

CUartParser::OperationType CUartParser::stringToOperationType(char *data)
{
	if(strcmp(data, "sayHello") == 0)
		return CUartParser::OperationType::SayHello;
	else if(strcmp(data, "setBlindType") == 0)
		return CUartParser::OperationType::SetBlindType;
	else if(strcmp(data, "getBlindType") == 0)
		return CUartParser::OperationType::GetBlindType;
	else if(strcmp(data, "setState") == 0)
		return CUartParser::OperationType::SetState;
	else if(strcmp(data, "getState") == 0)
		return CUartParser::OperationType::GetState;
	else if(strcmp(data, "calibrate") == 0)
		return CUartParser::OperationType::Calibrate;
	else
		return CUartParser::OperationType::UnhandledOperation;
}

const char *CUartParser::operationTypeToString(OperationType type)
{
	if(type == CUartParser::OperationType::SayHello)
		return "sayHello";
	else if(type == CUartParser::OperationType::SetBlindType)
		return "setBlindType";
	else if(type == CUartParser::OperationType::GetBlindType)
		return "getBlindType";
	else if(type == CUartParser::OperationType::SetState)
		return "setState";
	else if(type == CUartParser::OperationType::GetState)
		return "getState";
	else if(type == CUartParser::OperationType::Calibrate)
		return "calibrate";
	else
		return "";
}

bool CUartParser::validateRequest(char *data)
{
	if(strcmp(data, "request") == 0)
		return true;
	else
		return false;
}

bool CUartParser::prepareBaseResponse(CUartParser::OperationType type, char *content, char *buffer)
{
	if(buffer != nullptr)
	{
		strcpy(buffer, "#");
		strcat(buffer, operationTypeToString(type));
		strcat(buffer, "#response#");
		strcat(buffer, content);
		strcat(buffer, "#");

		return true;
	}
	else
		return false;

}

void CUartParser::processSayHello(CDataParser *parser)
{
	if(validateRequest(parser->getNextToken()))
	{
		char content[50];
		strcpy(content, "!");
		strcat(content, DEVICE_NAME);
		strcat(content, "_");
		strcat(content, DEVICE_ADDRESS);
		strcat(content, "!");

		char temp[3];
		itoa(BLIND_QUANTITY, temp, 10);
		strcat(content, temp);
		strcat(content, "!");

		char buffer[100];
		prepareBaseResponse(CUartParser::OperationType::SayHello, content, buffer);
		setResponseBuffer(buffer);
		setReadyForProcessResponse(true);
	}
}

void CUartParser::processGetBlindType(CDataParser *parser)
{
	if(validateRequest(parser->getNextToken()))
	{
		//Get blind number
		char *cContent = parser->getNextToken();
		CDataParser localParser;
		localParser.parseData(cContent, "!");

		uint8_t nBlindNo = atoi(localParser.getNextToken());
		if(nBlindNo == 1 || nBlindNo == 2)
		{
			char buffer[100];
			ServoEnum::BlindType blindType;
			if(nBlindNo == 1)
				blindType = m_servoModel1->getBlindType();
			else if(nBlindNo == 2)
				blindType = m_servoModel2->getBlindType();
			else
				return;

			char cBlindType[3];
			itoa(static_cast<int>(m_servoModel1->getBlindType()), cBlindType, 10);

			char content[50];
			strcpy(content, "!");
			strcat(content, cBlindType);
			strcat(content, "!");

			prepareBaseResponse(CUartParser::OperationType::GetBlindType, content, buffer);
			setResponseBuffer(buffer);
			setReadyForProcessResponse(true);
		}
	}
}

void CUartParser::event()
{
	if(isCheckUartMessage())
	{
		CDataParser parser;
		parser.parseData(m_pReceivedBuffer, "#");

		//Check operation name
		char *cOperationType = parser.getNextToken();
		if(stringToOperationType(cOperationType) == CUartParser::OperationType::SayHello)
		{
			processSayHello(&parser);
		}
		else if(stringToOperationType(cOperationType) == CUartParser::OperationType::GetBlindType)
		{
			processGetBlindType(&parser);
		}

		setCheckUartMessage(false);
	}

	if(isReadyForprocessResponse())
	{
		CUart::getInstance()->puts(getResponseBuffer());
		CUart::getInstance()->puts("\r");
		setReadyForProcessResponse(false);
	}
}
