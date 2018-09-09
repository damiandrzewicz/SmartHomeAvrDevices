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
			itoa(static_cast<int>(blindType), cBlindType, 10);

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

void CUartParser::processSetBlindType(CDataParser *parser)
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
			uint8_t nBlindType = atoi(localParser.getNextToken());
			ServoEnum::BlindType blindType = static_cast<ServoEnum::BlindType>(nBlindType);

			if(nBlindNo == 1)
				m_servoModel1->setBlindType(blindType);
			else if(nBlindNo == 2)
				m_servoModel2->setBlindType(blindType);
			else
				return;

			char buffer[100];
			char content[50];
			strcpy(content, "!ok!");

			prepareBaseResponse(CUartParser::OperationType::GetBlindType, content, buffer);
			setResponseBuffer(buffer);
			setReadyForProcessResponse(true);
		}
	}
}

void CUartParser::processGetState(CDataParser *parser)
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
			ServoEnum::Direction direction;
			uint8_t nOpenPercent;
			ServoEnum::Visibility visibility;
			bool bIsWindowClosed;

			char content[50];
			if(nBlindNo == 1)
			{
				direction = m_servoModel1->getDirection();
				nOpenPercent = m_servoModel1->getOpenPercent();
				visibility = m_servoModel1->getVisibility();
				bIsWindowClosed = m_servoModel1->isWindowClosed();
			}
			else if(nBlindNo == 2)
			{
				direction = m_servoModel1->getDirection();
				nOpenPercent = m_servoModel2->getOpenPercent();
				visibility = m_servoModel2->getVisibility();
				bIsWindowClosed = m_servoModel2->isWindowClosed();
			}
			else
				return;

			char temp[5];

			strcpy(content, "!");
			strcat(content, itoa(static_cast<uint8_t>(direction), temp, 10));
			strcat(content, "!");
			strcat(content, itoa(nOpenPercent, temp, 10));
			strcat(content, "!");
			strcat(content, itoa(static_cast<uint8_t>(visibility), temp, 10));
			strcat(content, "!");
			strcat(content, itoa(static_cast<uint8_t>(bIsWindowClosed), temp, 10));
			strcat(content, "!");

			char buffer[100];
			prepareBaseResponse(CUartParser::OperationType::GetState, content, buffer);
			setResponseBuffer(buffer);
			setReadyForProcessResponse(true);
		}
	}
}

void CUartParser::processSetState(CDataParser *parser)
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
			ServoEnum::Direction direction = static_cast<ServoEnum::Direction>(atoi(localParser.getNextToken()));
			uint8_t nOpenPercent = atoi(localParser.getNextToken());
			uint8_t nSpeedPercent = atoi(localParser.getNextToken());
			bool bAutoDrive = static_cast<bool>(atoi(localParser.getNextToken()));
			ServoEnum::Visibility visibility = static_cast<ServoEnum::Visibility>(atoi(localParser.getNextToken()));
			if(nBlindNo == 1)
			{
				m_servoModel1->setDirection(direction);
				m_servoModel1->setOpenPercent(nOpenPercent);
				m_servoModel1->setSpeedPercent(nSpeedPercent);
				m_servoModel1->setAutoDrive(bAutoDrive);
				m_servoModel1->setVisibility(visibility);
			}
			else if(nBlindNo == 2)
			{
				m_servoModel2->setDirection(direction);
				m_servoModel2->setOpenPercent(nOpenPercent);
				m_servoModel2->setSpeedPercent(nSpeedPercent);
				m_servoModel2->setAutoDrive(bAutoDrive);
				m_servoModel2->setVisibility(visibility);
			}
			else
				return;

			char buffer[100];
			char content[50];
			strcpy(content, "!ok!");

			prepareBaseResponse(CUartParser::OperationType::SetState, content, buffer);
			setResponseBuffer(buffer);
			setReadyForProcessResponse(true);
		}
	}
}

void CUartParser::processCalibrate(CDataParser *parser)
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
			//if(nBlindNo == 1)
				//m_servoModel1->
		}

	}
}

void CUartParser::event()
{
	if(isCheckUartMessage())
	{
		CInternalUartParser internalUartParser;
		internalUartParser.parseInputMessage(m_pReceivedBuffer);

		CInternalUartParser::MessageType messageType = internalUartParser.getIncomingMessageType();
		if(messageType == CInternalUartParser::MessageType::Request)
		{

			setCurrentOperationType(internalUartParser.getIncomingOperationType());
			strcpy(m_pReceivedBuffer, internalUartParser.getDataBuffer());
			setReadyForProcessUartMessage(true);
		}
		else
			return;

		setCheckUartMessage(false);
	}

	if(isReadyForProcessUartMessage())
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
		else if(stringToOperationType(cOperationType) == CUartParser::OperationType::SetBlindType)
		{
			processSetBlindType(&parser);
		}
		else if(stringToOperationType(cOperationType) == CUartParser::OperationType::GetState)
		{
			processGetState(&parser);
		}
		else if(stringToOperationType(cOperationType) == CUartParser::OperationType::SetState)
		{
			processSetState(&parser);
		}
		setReadyForProcessUartMessage(false);
	}

	if(isReadyForprocessResponse())
	{
		CInternalUartParser internalUartParser;
		internalUartParser.prepareOutputMessage(
				CInternalUartParser::MessageType::Response,
				getCurrentOperationType(),
				getResponseBuffer());


		CUart::getInstance()->puts(internalUartParser.getDataBuffer());
		CUart::getInstance()->puts("\r");
		setReadyForProcessResponse(false);
	}
}
