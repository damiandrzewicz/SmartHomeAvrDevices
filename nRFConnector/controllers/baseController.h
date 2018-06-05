/*
 * baseController.h
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

#ifndef CONTROLLERS_BASECONTROLLER_H_
#define CONTROLLERS_BASECONTROLLER_H_

#include <avr/io.h>
#include "../dataParser/dataParser.h"

//Friend functions - wrappers for specialised methods
void uartCallback(char *data);
void timerCallback();
void nrfCallback(void * nRF_RX_buff , uint8_t len );

class CBaseController {
public:
	CBaseController();
	virtual ~CBaseController();

	//Error types
	enum class ErrorType{
		Dummy = CDataParser::ParseResult::Ok,
		Timeout,
		General,
		OperationSpecified,
		Ok
	};

	//Constructor needs to be override for specify returned type
	virtual static CBaseController *getInstance() ;

	//Common interface
	void setOperationName(char *op);
	char *getOperationName();

	char *getBufferPtr();
	void setMessageInBuffer(char *msg);

	//Nrf interface
	void setRadioDataReceived(bool val);
	bool isRadioDataReceived();

	void processSendData();

	//Error handling
	void resetError();
	void setError(ErrorType err);
	void setError(int8_t err);
	bool isError();

	//Timer interface
	void incrementtimerTick();
	uint8_t getTimerHandle() const;
	bool isTimeout();
	void startTimer();
	void stopTimer();
	void resetTimerValue();

	virtual static void uartCallback(char *data);
	virtual static void timerCallback();
};

#endif /* CONTROLLERS_BASECONTROLLER_H_ */
