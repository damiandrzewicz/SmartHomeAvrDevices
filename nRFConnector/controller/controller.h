/*
 * controller.h
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#ifndef CONTROLLER_CONTROLLER_H_
#define CONTROLLER_CONTROLLER_H_

#include <avr/io.h>

void uartCallback(char *data);
void timerCallback();
void nrfCallback(void * nRF_RX_buff , uint8_t len );

class CController {
public:

	enum class ErrorGenre{
		NoError,
		General,
		OperationSpecified
	};

	static CController *getInstance();

	void setOperationName(char *op);
	char *getOperationName();

	void setErrorGenre(ErrorGenre err);
	bool isError();

	void setRequestInBufferReady(bool val);
	bool isRequestInBufferReady();

	void setRequestProcessReady(bool val);
	bool isRequestProcessReady();

	void setWaitingForRadioResponse(bool val);
	bool isWaitingForRadioResponse();

	void setRadioDataReceived(bool val);
	bool isRadioDataReceived();

	void setReadyForProcessResponse(bool val);
	bool isReadyForProcessResponse();

	void incrementtimerTick();

	uint8_t getTimerHandle() const;

	bool isTimeout();

	void startTimer();
	void stopTimer();
	void resetTimerValue();

	char *getBufferAddress();

	void prepareErrorNumber(int8_t err);

	void procesSetReceiverAddress();
	void processSendData();

	void controllerEvent();

	//Callbacks
	friend void uartCallback(char *data);
	friend void timerCallback();

private:
	//Cnstructors
	CController();	//Prevent consttuction but allow in getInstance
	CController(const CController&);	//Prevent construction by copying
	CController& operator=(const CController&);	//Prevent assigment
	~CController();					//Prevent unwanted destruction



private:
	static const uint8_t m_sBufferSize = 32;
	static const uint16_t m_sTimeout = 100;			//Timeout 500ms
	static CController * m_sInstance;
	static bool m_bIsCreated;
	static const uint8_t m_sTimerHandle = 0;

	volatile uint16_t m_nTimerValue = 0;
	char *m_sOperationName;
	char m_dataBuffer[m_sBufferSize];


	//********Flags*********

	/*
	 * Error flag
	 * 0 - no error
	 * 1 - general error
	 * 2 - operation specified
	 */
	ErrorGenre m_bErrorGenre = ErrorGenre::NoError;
	bool m_bRequestInBufferReady = false;
	bool m_bRequestProcessReady = false;
	bool m_bWaitingForRadioResponse = false;
	bool m_bRadioDataReceived = false;
	bool m_bReadyForProcessResponse = false;

};



#endif /* CONTROLLER_CONTROLLER_H_ */
