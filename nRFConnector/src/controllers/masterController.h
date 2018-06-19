/*
 * controller.h
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#pragma once

#include <avr/io.h>
#include "baseController.h"
#include "../dataParser/dataParser.h"

class CMasterController : public CBaseController<CMasterController>
{
	friend class CBaseController<CMasterController>;
private:
	CMasterController();
	CMasterController(const CMasterController&) = delete;
	CMasterController &operator=(const CMasterController&) = delete;
	~CMasterController();

public:
	void setReadyForProcessUartParsedMessage(bool value){ m_bReadyForProcessUartParsedMessage = value; }
	bool isReadyForProcessUartParsedMessage(){ return m_bReadyForProcessUartParsedMessage; }

	//-----------



//	void setRequestProcessReady(bool val);
//	bool isRequestProcessReady();

	void setWaitingForRadioResponse(bool val){ m_bWaitingForRadioResponse = val; }
	bool isWaitingForRadioResponse(){ return m_bWaitingForRadioResponse; }

//	void setReadyForProcessResponse(bool val);
//	bool isReadyForProcessResponse();


	//void procesSetReceiverAddress();
	//void prepareResponseMsgFromBuffer(char *buff);

	//Callbacks
	//void uartCallback(char *data);
	//void timerCallback() override;


	//Override controller
	void controllerEvent() override;

	//Operations
	void processSendDataToAir();
	void processRequest();
	void processResponse();

private:
	bool m_bReadyForProcessUartParsedMessage = false;
	bool m_bWaitingForRadioResponse = false;

	//bool m_bRequestInBufferReady = false;
	//bool m_bRequestProcessReady = false;


	//bool m_bReadyForProcessResponse = false;
};

/*
class CController {
public:



#ifdef IS_MASTER
	void setRequestInBufferReady(bool val);
	bool isRequestInBufferReady();

	void setRequestProcessReady(bool val);
	bool isRequestProcessReady();

	void setWaitingForRadioResponse(bool val);
	bool isWaitingForRadioResponse();

	void setReadyForProcessResponse(bool val);
	bool isReadyForProcessResponse();

	void procesSetReceiverAddress();
#else

#endif

	//Callbacks
	friend void uartCallback(char *data);
	friend void timerCallback();


#ifdef IS_MASTER
	bool m_bRequestInBufferReady = false;
	bool m_bRequestProcessReady = false;
	bool m_bWaitingForRadioResponse = false;

	bool m_bReadyForProcessResponse = false;
#else

#endif

};

*/
