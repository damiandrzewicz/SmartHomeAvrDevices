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



template<typename T>
class CBaseController {
protected:
	CBaseController();							//Prevent consttuction but allow in getInstance
	CBaseController(const CBaseController&) = delete;			//Prevent construction by copying
	CBaseController& operator=(const CBaseController&) = delete;	//Prevent assigment
	virtual ~CBaseController();							//Prevent unwanted destruction

public:
	//Get instance
	static T *getInstance()
	{
		static T instance;
		return &instance;
	}

	//Error types
	enum class ErrorType{
		Dummy = ParseResult::Ok,
		Timeout,
		General,
		OperationSpecified,
		Ok
	};

	//Common interface
	void setOperationName(char *op);
	char *getOperationName();

	char *getBufferPtr();
	void setMessageInBuffer(char *msg);

	//Uart
	void setReadyForProcessUart(bool value);
	bool isReadyForProcessUart();

	void setDataInUartBuffer(char *data);
	char *getUartdataBuffer();

	//Nrf interface
	void setRadioDataReceived(bool val);
	bool isRadioDataReceived();
	virtual void processSendData();

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

	//Callbacks
	//virtual void uartCallback(char *data);
	void timerCallback();
	void nrfCallback(void * nRF_RX_buff , uint8_t len );

	virtual void controllerEvent() = 0;

private:

	static const uint8_t m_sBufferSize = 32;
	static const uint16_t m_sTimeout = 100;			//Timeout 500ms
	static const uint8_t m_sTimerHandle = 0;

	volatile uint16_t m_nTimerValue = 0;
	char *m_sOperationName = nullptr;
	char m_dataBuffer[m_sBufferSize];
	char *m_uartBuffer = nullptr;

	int8_t m_nErrorNo;

	//********Flags*********
	bool m_bRadioDataReceived = false;
	bool m_bReadyForProcessUart = false;
};

#include "baseController.cpp"

#endif /* CONTROLLERS_BASECONTROLLER_H_ */
