/*
 * slaveController.h
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

#ifndef CONTROLLERS_SLAVECONTROLLER_H_
#define CONTROLLERS_SLAVECONTROLLER_H_

#include "baseController.h"
#include "../dataParser/dataParser.h"

//void uartCallback(char *data);
//void timerCallback();
//void nrfCallback(void * nRF_RX_buff , uint8_t len );

class CSlaveController : public CBaseController<CSlaveController>
{
	friend class CBaseController<CSlaveController>;
private:
	CSlaveController();
	CSlaveController(const CSlaveController&) = delete;
	CSlaveController &operator=(const CSlaveController&) = delete;
	~CSlaveController();

public:
	void processSendData() override;

	//Callbacks
	void uartCallback(char *data) override;
	void timerCallback() override;
	void nrfCallback(void * nRF_RX_buff , uint8_t len ) override;

	void controllerEvent() override;
};

#endif /* CONTROLLERS_SLAVECONTROLLER_H_ */
