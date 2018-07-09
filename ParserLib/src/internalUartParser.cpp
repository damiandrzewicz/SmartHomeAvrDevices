/*
 * internalConnParser.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#include <string.h>
#include "internalUartParser.h"


CInternalUartParser::CInternalUartParser() {
	// TODO Auto-generated constructor stub

}

CInternalUartParser::~CInternalUartParser() {
	// TODO Auto-generated destructor stub
}

void CInternalUartParser::cleanDataBuffer()
{
	memset(m_buffer, 0, m_sBufferRows);

}

char *CInternalUartParser::getDataBuffer()
{
	return m_buffer;
}

void CInternalUartParser::clean()
{
	cleanDataBuffer();
	setIncomingMessageType(MessageType::Unspecified);
	setIncomingOperationType(OperationType::Unspecified);
	setIncomingMessageValidate(MessageValidate::Unspecified);
}

CInternalUartParser::MessageType CInternalUartParser::getIncomingMessageType()
{
	return m_incomingMessageType;
}

CInternalUartParser::OperationType CInternalUartParser::getIncomingOperationType()
{
	return m_incomingOperationType;
}

void CInternalUartParser::setIncomingMessageType(CInternalUartParser::MessageType type)
{
	m_incomingMessageType = type;
}

void CInternalUartParser::setIncomingOperationType(CInternalUartParser::OperationType type)
{
	m_incomingOperationType = type;
}

const char *CInternalUartParser::getMessageTypeString(MessageType type) const
{
	if(type == MessageType::Request)
		return "request";
	else if(type == MessageType::Response)
		return "response";
	else
		return "unhandledType";
}

const char *CInternalUartParser::getOperationTypeString(OperationType type) const
{
	if(type == OperationType::PassData)
		return "passData";
	else if(type == OperationType::SendDataToAir)
		return "sendData";
	else if(type == OperationType::SetTransmitterAddress)
		return "setTrAddr";
	else
		return "unhandledType";
}

CInternalUartParser::MessageValidate CInternalUartParser::getIncomingMessageValidate()
{
	return m_incomingMessageValidate;
}

void CInternalUartParser::setIncomingMessageValidate(MessageValidate type)
{
	m_incomingMessageValidate = type;
}


char * CInternalUartParser::prepareOutputMessage(MessageType msgType, OperationType operationType, char *content, MessageValidate state)
{
	cleanDataBuffer();

	//Prepare operation type
	strcpy(m_buffer, "@");
	strcat(m_buffer, getOperationTypeString(operationType));
	strcat(m_buffer, "@");

	//Prepare message type
	strcat(m_buffer, getMessageTypeString(msgType));

	//Check error validation
	if(state == MessageValidate::Ok)
	{
		strcat(m_buffer, "@ok@");
	}
	else
	{
		strcat(m_buffer, "@err@");
	}

	//Prepare content
	if(content != nullptr)
	{
		strcat(m_buffer, content);
		strcat(m_buffer, "@");
	}


	return m_buffer;
}

UartParserResult CInternalUartParser::parseInputMessage(char *msg)
{
	cleanDataBuffer();

	//CUart::getInstance()->puts(msg);
	//CUart::getInstance()->puts("\r\n");

	//Message received, process it
	CDataParser parser;
	DataParseResult result = parser.parseData(msg, "@");

	//Check if message has been processed without errors
	if(result != DataParseResult::Ok)
	{
		return UartParserResult::ParsingError;
	}

	//Check operation type
	char *cOperationType = parser.getNextToken();
	//CUart::getInstance()->puts(cOperationType);
	//CUart::getInstance()->puts("\r");
	if(strcmp(cOperationType, getOperationTypeString(OperationType::PassData)) == 0)
	{
		setIncomingOperationType(OperationType::PassData);
	}
	else if(strcmp(cOperationType, getOperationTypeString(OperationType::SendDataToAir)) == 0)
	{
		setIncomingOperationType(OperationType::SendDataToAir);
	}
	else if(strcmp(cOperationType, getOperationTypeString(OperationType::SetTransmitterAddress)) == 0)
	{
		setIncomingOperationType(OperationType::SetTransmitterAddress);
	}
	else
	{
		setIncomingOperationType(OperationType::Unspecified);
		return UartParserResult::BadOperationType;
	}

	//Check messageType
	char *cMessageType = parser.getNextToken();
	if(strcmp(cMessageType, getMessageTypeString(MessageType::Request)) == 0)
	{
		setIncomingMessageType(MessageType::Request);
	}
	else if(strcmp(cMessageType, getMessageTypeString(MessageType::Response)) == 0)
	{
		setIncomingMessageType(MessageType::Response);
	}
	else
	{
		setIncomingMessageType(MessageType::Unspecified);
		return UartParserResult::BadMessageType;
	}

	char *cData = parser.getNextToken();		//Get data
	if(cData == NULL || strcmp(cData, "") == 0)
	{
		return UartParserResult::EmptyData;
	}

	if(getIncomingMessageType() == MessageType::Request)
	{
		strcpy(m_buffer, cData);					//Save data in buffer
	}
	else if(getIncomingMessageType() == MessageType::Response)
	{
		//Check Validation
		if(strcmp(cData, "ok") == 0)
		{
			setIncomingMessageValidate(MessageValidate::Ok);
		}
		else if(strcmp(cData, "err") == 0)
		{
			setIncomingMessageValidate(MessageValidate::Error);
			// TODO - implement error number handling
		}
		else
		{
			setIncomingMessageValidate(MessageValidate::Unspecified);
		}
	}

	//CUart::getInstance()->puts(cRequest);
	//CUart::getInstance()->puts("\r\n");

	return UartParserResult::Ok;
}

