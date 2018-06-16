/*
 * controller.h
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#ifndef CONTROLLERS_MASTERCONTROLLER_H_
#define CONTROLLERS_MASTERCONTROLLER_H_

#include <avr/io.h>
#include "baseController.h"
#include "../dataParser/dataParser.h"

//void uartCallback(char *data);
//void timerCallback();
//void nrfCallback(void * nRF_RX_buff , uint8_t len );

class CMasterController : public CBaseController<CMasterController>
{
	friend class CBaseController<CMasterController>;
private:
	CMasterController();
	CMasterController(const CMasterController&) = delete;
	CMasterController &operator=(const CMasterController&) = delete;
	~CMasterController();

public:
	void setRequestInBufferReady(bool val);
	bool isRequestInBufferReady();

	void setRequestProcessReady(bool val);
	bool isRequestProcessReady();

	void setWaitingForRadioResponse(bool val);
	bool isWaitingForRadioResponse();

	void setReadyForProcessResponse(bool val);
	bool isReadyForProcessResponse();

	void processSendData() override;
	void procesSetReceiverAddress();
	void prepareResponseMsgFromBuffer(char *buff);

	//Callbacks
	void uartCallback(char *data);
	//void timerCallback() override;


	void controllerEvent() override;

private:
	bool m_bRequestInBufferReady = false;
	bool m_bRequestProcessReady = false;
	bool m_bWaitingForRadioResponse = false;

	bool m_bReadyForProcessResponse = false;
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

#endif /* CONTROLLERS_MASTERCONTROLLER_H_ */
