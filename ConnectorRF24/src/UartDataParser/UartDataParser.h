/*
 * UartDataParser.h
 *
 *  Created on: Sep 3, 2018
 *      Author: damian
 */

#pragma once

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "ParserInterface/CParserInterface.h"



class CUartDataParser : public CParserInterface  {
public:
	enum class OperationName{
		SendDataToDevice,
		SayHello, //Name and address
		SetConnectorAddress,
		GetConnectorAddress,
		RestartConnector,
		NotSupported
	};

	CUartDataParser();
	virtual ~CUartDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult = nullptr);

	virtual bool createErrorMsg(uint8_t err, char *pResult) override;

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

protected:
	void setOperationName(OperationName op);

	OperationName parseOperationName(char *pOperationName);

private:
	//OperationName m_operationName;
};

