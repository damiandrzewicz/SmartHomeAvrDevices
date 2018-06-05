/*
 * controller.h
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#ifndef CONTROLLERS_MASTERCONTROLLER_H_
#define CONTROLLERS_MASTERCONTROLLER_H_

#include <avr/io.h>
#include "../dataParser/dataParser.h"

void uartCallback(char *data);
void timerCallback();
void nrfCallback(void * nRF_RX_buff , uint8_t len );

class CController {
public:

	enum class ErrorType{
		Dummy = CDataParser::ParseResult::Ok,
		Timeout,
		General,
		OperationSpecified,
		Ok
	};

	static CController *getInstance();

	void setOperationName(char *op);
	char *getOperationName();

	void setRadioDataReceived(bool val);
	bool isRadioDataReceived();

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


	void processSendData();

	void resetError();
	void setError(ErrorType err);
	void setError(int8_t err);
	bool isError();

	void incrementtimerTick();

	uint8_t getTimerHandle() const;

	bool isTimeout();

	void startTimer();
	void stopTimer();
	void resetTimerValue();

	char *getBufferPtr();
	void setMessageInBuffer(char *msg);

	void controllerEvent();

	//Callbacks
	friend void uartCallback(char *data);
	friend void timerCallback();

private:
	//Cnstructors
	CController();								//Prevent consttuction but allow in getInstance
	CController(const CController&);			//Prevent construction by copying
	CController& operator=(const CController&);	//Prevent assigment
	~CController();								//Prevent unwanted destruction

private:
	static const uint8_t m_sBufferSize = 32;
	static const uint16_t m_sTimeout = 100;			//Timeout 500ms
	static CController * m_sInstance;
	static bool m_bIsCreated;
	static const uint8_t m_sTimerHandle = 0;

	volatile uint16_t m_nTimerValue = 0;
	char *m_sOperationName;
	char m_dataBuffer[m_sBufferSize];

	//ErrorType m_nErrorNo = ErrorType::Ok;
	int8_t m_nErrorNo;

	//********Flags*********
	bool m_bRadioDataReceived = false;

#ifdef IS_MASTER
	bool m_bRequestInBufferReady = false;
	bool m_bRequestProcessReady = false;
	bool m_bWaitingForRadioResponse = false;

	bool m_bReadyForProcessResponse = false;
#else

#endif

};



#endif /* CONTROLLERS_MASTERCONTROLLER_H_ */
