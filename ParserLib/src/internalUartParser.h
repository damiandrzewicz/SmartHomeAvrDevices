/*
 * internalConnParser.h
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#pragma once

#include <avr/io.h>
#include "dataParser.h"
#include "resultTypes.h"

class CInternalUartParser {
public:

	/*
	 *
	 */
	enum class MessageType{ Request, Response, Unspecified };
	enum class OperationType{ SetTransmitterAddress, SendDataToAir, PassData, Unspecified };
	enum class MessageValidate { Error, Ok, Unspecified };

	CInternalUartParser();
	~CInternalUartParser();

	char * prepareOutputMessage(MessageType msgType, OperationType operationType, char *content, MessageValidate state = MessageValidate::Ok);
	UartParserResult parseInputMessage(char *msg);

	//void prepareOkContent(char *data);
	//void prepareErrorContent(uint8_t errNb);


	MessageType getIncomingMessageType();
	OperationType getIncomingOperationType();
	MessageValidate getIncomingMessageValidate();

	char *getDataBuffer();
	void clean();

	const char *getMessageTypeString(MessageType type) const;
	const char *getOperationTypeString(OperationType type) const;


	//Operations
private:
	void setIncomingMessageType(MessageType type);
	void setIncomingOperationType(OperationType type);
	void setIncomingMessageValidate(MessageValidate type);
	void cleanDataBuffer();

private:
	static const uint8_t m_sBufferRows = 55;
	char m_buffer[m_sBufferRows];

	// Request/response
	MessageType m_incomingMessageType = MessageType::Unspecified;
	OperationType m_incomingOperationType = OperationType::Unspecified;
	MessageValidate m_incomingMessageValidate = MessageValidate::Unspecified;
};


