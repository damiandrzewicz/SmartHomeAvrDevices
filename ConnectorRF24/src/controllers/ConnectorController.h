/*
 * ConnectorController.h
 *
 *  Created on: Aug 28, 2018
 *      Author: damian
 */

#pragma once

#include "BaseController.h"

class CConnectorController : public CBaseController {
public:
	CConnectorController();
	virtual ~CConnectorController();

	virtual void eventLoop() override;

private:
	CUartDataParser::OperationName m_uartOperationName;
	CRadioDataParser::OperationName m_radioOperationName;

	bool m_bWaitingForUartResponse = false;
	bool m_bProcessRadioResponseStep = false;
};

