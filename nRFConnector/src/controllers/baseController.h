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
#include "../dataParser/internalUartParser.h"
#include "../dataParser/resultTypes.h"


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

	//Process data
	// TODO - check if this will be important to use
	void setBusy(bool value);
	bool isBusy();

	char *getBufferPtr();
	void setDataInBuffer(char *msg);

	//Uart
	void setReadyForProcessUartReceivedData(bool value){ m_bReadyForProcessReceivedUartData = value; }
	bool isReadyForProcessUartReceivedData(){ return m_bReadyForProcessReceivedUartData; }

	void setReadyForProcessUartResponseData(bool value){ m_bReadyForProcessUartResponseData = value; }
	bool isReadyForProcessUartResponseData(){ return m_bReadyForProcessUartResponseData; }

	//Radio data
	void setReadyForProcessReceivedRadioData(bool value){ m_bReadyForProcessReceivedRadioData = value; }
	bool isReadyForProcessReceivedRadioData(){ return m_bReadyForProcessReceivedRadioData; }

	void setWaitingForUartResponse(bool value){ m_bIsWaitingForUartResponse = value; }
	bool isWaitingForUartResponse(){ return m_bIsWaitingForUartResponse; }

	void setCurrentOperationType(CInternalUartParser::OperationType type){ m_currentOperationType = type; }
	CInternalUartParser::OperationType getCurrentOperationType(){ return m_currentOperationType; }

	//Error handling
	void resetError();
	void setError(int8_t err);
	int8_t getErrorNo();
	bool isError();

	//Timer interface
	void incrementtimerTick();
	uint8_t getTimerHandle() const;
	bool isTimeout();
	void startTimer();
	void stopTimer();
	void resetTimerValue();

	//Callbacks
	void uartCallback(char *data);
	void timerCallback();
	void nrfCallback(void * nRF_RX_buff , uint8_t len );

	//Main event - to implement in inherited classes
	virtual void controllerEvent() = 0;

	//Operations
	virtual void processSendDataToAir();
	virtual void processSetTransmitterAddress();

	void clean();

protected:
	CInternalUartParser m_internalUartParser;

private:
	static const uint8_t m_sBufferSize = 32;
	static const uint16_t m_sTimeout = 100;			//Timeout 500ms
	static const uint8_t m_sTimerHandle = 0;

	volatile uint16_t m_nTimerValue = 0;
	char m_dataBuffer[m_sBufferSize];

	int8_t m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);
	//ControllerResult m_controllerState = ControllerResult::Dummy;

	CInternalUartParser::OperationType m_currentOperationType = CInternalUartParser::OperationType::Unspecified;

	//********Flags*********
	volatile bool m_bReadyForProcessReceivedRadioData = false;
	volatile bool m_bReadyForProcessReceivedUartData = false;
	volatile bool m_bIsWaitingForUartResponse = false;
	volatile bool m_bReadyForProcessUartResponseData = false;
};

#include "baseController.cpp"

#endif /* CONTROLLERS_BASECONTROLLER_H_ */
