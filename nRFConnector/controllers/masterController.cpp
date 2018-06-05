/*
 * controller.cpp
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#include "masterController.h"

#include <string.h>
#include "../nrf/nrf.h"
#include "../nrf/nRF24L01_memory_map.h"
#include "../uart/uart.h"
#include "../timer/timer.h"

CController *CController::m_sInstance = nullptr;
bool CController::m_bIsCreated = false;

CController *CController::getInstance()
{
	if(!m_bIsCreated)
	{
		static CController controller;
		m_sInstance = &controller;
		m_bIsCreated = true;
	}
	return m_sInstance;
}

CController::CController(){
	m_nErrorNo = static_cast<int8_t>(ErrorType::Ok);

}

CController::~CController() {
	// TODO Auto-generated destructor stub
}

void CController::setOperationName(char *op)
{
	m_sOperationName = op;
}

char *CController::getOperationName()
{
	return m_sOperationName;
}

void CController::resetError()
{
	setError(ErrorType::Ok);
}

void CController::setError(ErrorType err)
{
	setError(static_cast<int8_t>(err));
}

void CController::setError(int8_t err)
{
	m_nErrorNo = static_cast<int8_t>(err);
	char valBuff[5];
	itoa(static_cast<int8_t>(m_nErrorNo), valBuff, 10);
	strcpy(getBufferPtr(), valBuff);
}

bool CController::isError()
{
	int8_t nOk = static_cast<int8_t>(ErrorType::Ok);
	return (m_nErrorNo != nOk);
}

#ifdef IS_MASTER
void CController::setRequestInBufferReady(bool val)
{
	m_bRequestInBufferReady = val;
}

bool CController::isRequestInBufferReady()
{
	return m_bRequestInBufferReady;
}

void CController::setRequestProcessReady(bool val)
{
	m_bRequestProcessReady = val;
}

bool CController::isRequestProcessReady()
{
	return m_bRequestProcessReady;
}

void CController::setWaitingForRadioResponse(bool val)
{
	m_bWaitingForRadioResponse = val;
}

bool CController::isWaitingForRadioResponse()
{
	return m_bWaitingForRadioResponse;
}

void CController::setReadyForProcessResponse(bool val)
{
	m_bReadyForProcessResponse = val;
}

bool CController::isReadyForProcessResponse()
{
	return m_bReadyForProcessResponse;
}
#else

#endif

void CController::setRadioDataReceived(bool val)
{
	m_bRadioDataReceived = val;
}

bool CController::isRadioDataReceived()
{
	return m_bRadioDataReceived;
}

uint8_t CController::getTimerHandle() const
{
	return m_sTimerHandle;
}

void CController::startTimer()
{
	resetTimerValue();
	CTimer2::getInstance()->Enable(getTimerHandle());
}

void CController::stopTimer()
{
	CTimer2::getInstance()->Disable(getTimerHandle());
}

void CController::resetTimerValue()
{
	m_nTimerValue = 0;
}

void CController::incrementtimerTick()
{
	m_nTimerValue++;
}

char *CController::getBufferPtr()
{
	return m_dataBuffer;
}

void CController::setMessageInBuffer(char *msg)
{
	strcpy(CController::getInstance()->getBufferPtr(), msg);
}

bool CController::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}

void CController::procesSetReceiverAddress()
{
	CNrf::getInstance()->setTransmitterAdres(m_dataBuffer);
	CNrf::getInstance()->setReciverAddres(RX_ADDR_P0, m_dataBuffer);
	//PORTC ^= (1 << PC5);
}

void CController::processSendData()
{
	CNrf::getInstance()->sendDataToAir(m_dataBuffer);
	setWaitingForRadioResponse(true);
	startTimer();
}


void CController::controllerEvent()
{
#ifdef IS_MASTER
	if(isRequestInBufferReady())
	{
		if(strcmp(getOperationName(), "setReceiverAddress") == 0)
		{
			//Process "setReceiverAddress" message
			procesSetReceiverAddress();
			setReadyForProcessResponse(true);
		}
		else if(strcmp(getOperationName(), "sendData") == 0)
		{
			//Process "sendData" message
			processSendData();
		}

		setRequestInBufferReady(false);
	}

	if(isWaitingForRadioResponse())
	{

		if(isRadioDataReceived())
		{
			stopTimer();
			setRadioDataReceived(false);
			setWaitingForRadioResponse(false);

			setReadyForProcessResponse(true);
		}
		else if(isTimeout())
		{

			stopTimer();
			setError(ErrorType::Timeout);

			setRadioDataReceived(false);
			setWaitingForRadioResponse(false);

			setReadyForProcessResponse(true);;
		}
	}

	if(isReadyForProcessResponse())
	{
		//Prepare response
		char response[100];
		strcpy(response, "@response");

		//Check if Error occured
		if(isError())
		{
			strcat(response, "@err@");
		}
		else
		{
			strcat(response, "@ok@");
		}

		//Add info data
		strcat(response, m_dataBuffer);


		if(strcmp(getOperationName(), "") != 0)
		{
			strcat(response, "@");
			strcat(response, getOperationName());
		}

		strcat(response, "@");

		CUart::getInstance()->puts(response);			//Send response by UART
		CUart::getInstance()->puts("\r\n");				//Terminate response

		//Reset ready for response state
		setReadyForProcessResponse(false);

		//Reset error if occured
		if(isError())
		{
			resetError();
		}
	}
#else
	if(isRadioDataReceived())
	{
		CUart::getInstance()->puts(m_dataBuffer);		//Send response by UART
		CUart::getInstance()->puts("\r\n");				//Terminate response
		setRadioDataReceived(false);
	}
#endif
}

//Callbacks
void uartCallback(char *data)
{
	//Message received, process it
	CDataParser parser;
	CDataParser::ParseResult result = parser.parseData(data);

	if(result != CDataParser::ParseResult::Ok)
	{
		CController::getInstance()->setError(static_cast<int8_t>(result));

		CController::getInstance()->setReadyForProcessResponse(true);
		return;
	}

	char *cRequest = parser.getNextToken();

	if(strcmp(cRequest, "request") != 0)
	{
		CController::getInstance()->setError(CController::ErrorType::General);
		CController::getInstance()->setReadyForProcessResponse(true);
		return;
	}

	CController::getInstance()->setOperationName(parser.getNextToken());

	//Process set receiver address operation
	if(strcmp(CController::getInstance()->getOperationName(), "setReceiverAddress") == 0)
	{
		//Get value
		char *cValue = parser.getNextToken();
		char newAddr[6];
		strcpy(newAddr, "x");
		strcat(newAddr, cValue);

		CController::getInstance()->setMessageInBuffer(newAddr);
		CController::getInstance()->setRequestInBufferReady(true);
	}
	//Process send data to air
	else if(strcmp(CController::getInstance()->getOperationName(), "sendData") == 0)
	{
		//Get message
		char *cMessage = parser.getNextToken();

		CController::getInstance()->setMessageInBuffer(cMessage);
		CController::getInstance()->setRequestInBufferReady(true);
	}
}

void nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	CController::getInstance()->setMessageInBuffer(static_cast<char*>(nRF_RX_buff));
	CController::getInstance()->setRadioDataReceived(true);
}

void timerCallback()
{
	CController::getInstance()->incrementtimerTick();
}


