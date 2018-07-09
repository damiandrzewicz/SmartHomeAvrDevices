/*
 * controller.h
 *
 *  Created on: Jun 28, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_CONTROLLERTERMINAL_H_
#define SRC_CONTROLLERS_CONTROLLERTERMINAL_H_

#include "../dataParser/internalUartParser.h"

class CControllerTerminal {

private:
	CControllerTerminal();							//Prevent consttuction but allow in getInstance
	CControllerTerminal(const CControllerTerminal&) = delete;			//Prevent construction by copying
	CControllerTerminal& operator=(const CControllerTerminal&) = delete;	//Prevent assigment
	~CControllerTerminal();							//Prevent unwanted destruction

public:
	//Get instance
	static CControllerTerminal *getInstance()
	{
		static CControllerTerminal instance;
		return &instance;
	}

	//Operations
public:

	//Signals
	//Uart
	void setReadyForProcessUartReceivedData(bool value){ m_bReadyForProcessReceivedUartData = value; }
	volatile bool isReadyForProcessUartReceivedData(){ return m_bReadyForProcessReceivedUartData; }

	void setReadyForProcessUartResponseData(bool value){ m_bReadyForProcessUartResponseData = value; }
	volatile bool isReadyForProcessUartResponseData(){ return m_bReadyForProcessUartResponseData; }

	//Radio data
	void setReadyForProcessReceivedRadioData(bool value){ m_bReadyForProcessReceivedRadioData = value; }
	volatile bool isReadyForProcessReceivedRadioData(){ return m_bReadyForProcessReceivedRadioData; }

	void setWaitingForUartResponse(bool value){ m_bIsWaitingForUartResponse = value; }
	volatile bool isWaitingForUartResponse(){ return m_bIsWaitingForUartResponse; }

	void setReadyForProcessUartParsedMessage(bool value){ m_bReadyForProcessUartParsedMessage = value; }
	volatile bool isReadyForProcessUartParsedMessage(){ return m_bReadyForProcessUartParsedMessage; }

	void setWaitingForRadioResponse(bool val){
		m_bWaitingForRadioResponse = val;
		startTimer();
	}
	volatile bool isWaitingForRadioResponse(){ return m_bWaitingForRadioResponse; }

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

	void processRequest();
	void processSetTransmitterAddress();
	void processSendDataToAir();

	//Callbacks
	void uartCallback(char *data);
	void timerCallback();
	void nrfCallback(void * nRF_RX_buff , uint8_t len );

	//Main event
	void controllerEvent();


	//Memebrs
private:

	CInternalUartParser m_internalUartParser;

	static const uint8_t m_sBufferSize = 32;
	static const uint16_t m_sTimeout = 100;			//Timeout 500ms
	static const uint8_t m_sTimerHandle = 0;

	volatile uint16_t m_nTimerValue = 0;
	char m_dataBuffer[m_sBufferSize];
	char *m_pReceivedBuffer = nullptr;

	int8_t m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);

	CInternalUartParser::OperationType m_currentOperationType = CInternalUartParser::OperationType::Unspecified;

	//Flags
	volatile bool m_bReadyForProcessReceivedRadioData = false;
	volatile bool m_bReadyForProcessReceivedUartData = false;
	volatile bool m_bIsWaitingForUartResponse = false;
	volatile bool m_bReadyForProcessUartResponseData = false;
	volatile bool m_bReadyForProcessUartParsedMessage = false;
	volatile bool m_bWaitingForRadioResponse = false;
};

#endif /* SRC_CONTROLLERS_CONTROLLERTERMINAL_H_ */
