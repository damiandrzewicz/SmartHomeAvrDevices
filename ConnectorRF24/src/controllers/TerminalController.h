/*
 * TerminalController.h
 *
 *  Created on: Aug 28, 2018
 *      Author: damian
 */

#pragma once

#include "BaseController.h"

class CTerminalController : public CBaseController {
public:
	CTerminalController();
	virtual ~CTerminalController();

	virtual void eventLoop() override;
};


