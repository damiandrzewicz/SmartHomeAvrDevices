/*
 * CStandaloneController.h
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_STANDALONECONTROLLER_H_
#define SRC_CONTROLLERS_STANDALONECONTROLLER_H_

#include "baseController.h"
#include "../dataParser/dataParser.h"

class CStandaloneController : public CBaseController<CStandaloneController>
{
	friend class CBaseController<CStandaloneController>;
private:
	CStandaloneController();
	CStandaloneController(const CStandaloneController&) = delete;
	CStandaloneController &operator=(const CStandaloneController&) = delete;
	~CStandaloneController();

public:
	void processSendData() override;
	void prepareResponseMsgFromBuffer(char *buff);

	void setRequestInBufferReady(bool val);
	bool isRequestInBufferReady();

	void setReadyForProcessResponse(bool val);
	bool isReadyForProcessResponse();

	//Callbacks
	void uartCallback(char *data);
	void controllerEvent() override;

private:
	bool m_bRequestInBufferReady = false;
	bool m_bReadyForProcessResponse = false;
};

#endif /* SRC_CONTROLLERS_STANDALONECONTROLLER_H_ */
