/*
 * UartDataParser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#pragma once

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "ParserInterface/CParserInterface.h"

class CConnectorUartDataParser : public CParserInterface  {
public:
	enum class OperationName{
		SendDataToDevice,
		SetConnectorAddress,
		GetConnectorAddress,
		PowerUpConnector,
		PowerDownConnector,
		NotSupported
	};

	CConnectorUartDataParser();
	~CConnectorUartDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult = nullptr);

	virtual bool createErrorMsg(uint8_t errNo, char *pResult) override;

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

protected:
	void setOperationName(OperationName op);

	OperationName parseOperationName(char *pOperationName);

private:
	//OperationName m_operationName;
};
