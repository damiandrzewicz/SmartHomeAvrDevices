/*
 * controller.cpp
 *
 *  Created on: Apr 8, 2018
 *      Author: damian
 */

#include <string.h>
#include "controller.h"
#include "../dataParser/dataParser.h"
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
	// TODO Auto-generated constructor stub

}

CController::~CController() {
	// TODO Auto-generated destructor stub
}

void CController::setOperationName(char *op)
{
	//strcpy(m_sOperationName, op);
	m_sOperationName = op;
}

char *CController::getOperationName()
{
	return m_sOperationName;
}

void CController::setErrorGenre(ErrorGenre err)
{
	m_bErrorGenre = err;
}

bool CController::isError()
{
	return static_cast<bool>(m_bErrorGenre);
}

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

void CController::setRadioDataReceived(bool val)
{
	m_bRadioDataReceived = val;
}

bool CController::isRadioDataReceived()
{
	return m_bRadioDataReceived;
}

void CController::setReadyForProcessResponse(bool val)
{
	m_bReadyForProcessResponse = val;
}

bool CController::isReadyForProcessResponse()
{
	return m_bReadyForProcessResponse;
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

	//CUart::getInstance()->putint(m_nTimerValue, 10);
	//CUart::getInstance()->puts("\n\r");
}


char *CController::getBufferAddress()
{
	return m_dataBuffer;
}


bool CController::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}



void CController::prepareErrorNumber(int8_t err)
{
	char valBuff[5];
	itoa(err, valBuff, 10);
	strcpy(getBufferAddress(), valBuff);
	return;
}

void CController::procesSetReceiverAddress()
{
	CNrf::getInstance()->setTransmitterAdres(m_dataBuffer);
	CNrf::getInstance()->setReciverAddres(RX_ADDR_P0, m_dataBuffer);
	PORTC ^= (1 << PC5);
}

void CController::processSendData()
{
	CNrf::getInstance()->sendDataToAir(m_dataBuffer);
	setWaitingForRadioResponse(true);
	startTimer();
}


void CController::controllerEvent()
{
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
			setErrorGenre(ErrorGenre::OperationSpecified);
			prepareErrorNumber(-20);

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
			setErrorGenre(ErrorGenre::NoError);		//Reset error state
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
		CUart::getInstance()->puts("\r");				//Terminate response
		CUart::getInstance()->puts("\n");
		PORTC ^= (1 << PC5);
		//Reset ready for response state
		setReadyForProcessResponse(false);
	}

}

//Callbacks
void uartCallback(char *data)
{
	//CController::getInstance()->stopTimer();
	//CUart::getInstance()->puts(/*response*/"tes");


	//Message received, process it
	CDataParser parser;
	int8_t result = parser.parseData(data);
	//CUart::getInstance()->putint(result, 10);
	if(result != 1)
	{
		CController::getInstance()->prepareErrorNumber(result);
		CController::getInstance()->setErrorGenre(CController::ErrorGenre::General);
		CController::getInstance()->setReadyForProcessResponse(true);

		//CUart::getInstance()->puts("blabl");
		return;
	}


	char *cRequest = parser.getNextToken();
//	CUart::getInstance()->puts("**");
//	CUart::getInstance()->puts(cRequest);
//	CUart::getInstance()->puts("**");
	if(strcmp(cRequest, "request") != 0)
	{
		CController::getInstance()->prepareErrorNumber(-15);
		CController::getInstance()->setErrorGenre(CController::ErrorGenre::General);
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

		strcpy(CController::getInstance()->getBufferAddress(), newAddr);
		CController::getInstance()->setRequestInBufferReady(true);
	}
	//Process send data to air
	else if(strcmp(CController::getInstance()->getOperationName(), "sendData") == 0)
	{
		//Get message
		char *cMessage = parser.getNextToken();

		strcpy(CController::getInstance()->getBufferAddress(), cMessage);
		CController::getInstance()->setRequestInBufferReady(true);
	}

}

void timerCallback()
{
	CController::getInstance()->incrementtimerTick();
	PORTC ^= (1 << PC5);
	//CNrf::getInstance()->sendDataToAir("Test damian :D to jest bardzo dlugi string i zobaczymy co bedzie i jak zostanie odebrany bo same jestem ciekaw");
}

void nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	//CUart::getInstance()->puts("Odebrano: ");
	//CUart::getInstance()->puts((char*)nRF_RX_buff);
	//CUart::getInstance()->puts("\n\r");

	strcpy(CController::getInstance()->getBufferAddress(), static_cast<char*>(nRF_RX_buff));

	//if(CController::getInstance()->isWaitingForRadioResponse())
		CController::getInstance()->setRadioDataReceived(true);

	//CController::getInstance()->stopTimer();

	//CController::getInstance()->setNrfReady(true);
}


