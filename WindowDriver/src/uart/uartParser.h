/*
 * uartParser.h
 *
 *  Created on: Jul 8, 2018
 *      Author: damian
 */

#ifndef SRC_UART_UARTPARSER_H_
#define SRC_UART_UARTPARSER_H_

#include "../servo/servo.h"
#include "../../ParserLib/src/dataParser.h"

class CUartParser {
public:

	enum class OperationType
	{
		SayHello,
		SetBlindType,
		GetBlindType,
		SetState,
		GetState,
		Calibrate,
		UnhandledOperation
	};

	CUartParser();
	~CUartParser();

	void setServoModel1(ServoModel *state);
	void setServoModel2(ServoModel *state);

	void setCheckUartMessage(bool value){ m_bCheckUartMessage = value; }
	volatile bool isCheckUartMessage(){ return m_bCheckUartMessage; }
	void setReadyForProcessResponse(bool value){ m_bReadyForProcessResponse = value; }
	bool isReadyForprocessResponse(){ return m_bReadyForProcessResponse; }

	void setResponseBuffer(char *data);
	char *getResponseBuffer();

	bool validateRequest(char *data);
	OperationType stringToOperationType(char *data);
	const char *operationTypeToString(OperationType type);
	bool prepareBaseResponse(CUartParser::OperationType type, char *content, char *buffer = nullptr);

	void uartCallback(char *data);

	void processSayHello(CDataParser *parser);
	void processGetBlindType(CDataParser *parser);

	void event();

private:
	ServoModel *m_servoModel1 = nullptr;
	ServoModel *m_servoModel2 = nullptr;

	char *m_pReceivedBuffer = nullptr;
	bool m_bCheckUartMessage = false;
	bool m_bReadyForProcessResponse = false;

	char m_responseBuffer[50];
};

#endif /* SRC_UART_UARTPARSER_H_ */
