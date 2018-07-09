/*
 * controllerDevice.h
 *
 *  Created on: Jun 29, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_CONTROLLERDEVICE_H_
#define SRC_CONTROLLERS_CONTROLLERDEVICE_H_

#include "../dataParser/internalUartParser.h"

class CControllerDevice {

private:
	CControllerDevice();							//Prevent consttuction but allow in getInstance
	CControllerDevice(const CControllerDevice&) = delete;			//Prevent construction by copying
	CControllerDevice& operator=(const CControllerDevice&) = delete;	//Prevent assigment
	~CControllerDevice();							//Prevent unwanted destruction

public:
	//Get instance
	static CControllerDevice *getInstance()
	{
		static CControllerDevice instance;
		return &instance;
	}

	void setCurrentOperationType(CInternalUartParser::OperationType type){ m_currentOperationType = type; }
	CInternalUartParser::OperationType getCurrentOperationType(){ return m_currentOperationType; }

	//Radio data
	void setReadyForProcessReceivedRadioData(bool value){ m_bReadyForProcessReceivedRadioData = value; }
	volatile bool isReadyForProcessReceivedRadioData(){ return m_bReadyForProcessReceivedRadioData; }

	void setReadyForProcessRadioResponse(bool value){ m_bReadyForProcessRadioResponse = value; }
	volatile bool isReadyForProcessRadioResponse(){ return m_bReadyForProcessRadioResponse; }

	//Uart
	void setWaitForUartData(bool value)
	{
		m_bWaitForUartData = value;
		if(value == true)
			startTimer();
	}
	volatile bool isWaitingForUartData(){ return m_bWaitForUartData; }

	void setReceivedUartData(bool value)
	{
		m_bReceivedUartData = value;
		if(value == true)
			stopTimer();
	}
	volatile bool isReceivedUartData(){ return m_bReceivedUartData; }

	void setCheckUartMessage(bool value){ m_bCheckUartMessage = value; }
	volatile bool isCheckUartMessage(){ return m_bCheckUartMessage; }

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

	//Main event
	void controllerEvent();

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
	volatile bool m_bWaitForUartData = false;
	volatile bool m_bReceivedUartData = false;
	volatile bool m_bReadyForProcessRadioResponse = false;
	volatile bool m_bCheckUartMessage = false;
};

#endif /* SRC_CONTROLLERS_CONTROLLERDEVICE_H_ */
