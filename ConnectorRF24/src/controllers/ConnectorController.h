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
};

