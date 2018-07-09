/*
 * controller.cpp
 *
 *  Created on: Jun 28, 2018
 *      Author: damian
 */

#include "controllerTerminal.h"

#include <string.h>
#include "../timer/timer.h"
#include "../uart/uart.h"
#include "../nrf/nrf.h"
#include "../nrf/nRF24L01_memory_map.h"

void debug1()
{
	CUart::getInstance()->puts("here");			//Send response by UART
}

CControllerTerminal::CControllerTerminal() {
	// TODO Auto-generated constructor stub

}

CControllerTerminal::~CControllerTerminal() {
	// TODO Auto-generated destructor stub
}

//******Main event******
void CControllerTerminal::controllerEvent()
{
	if(isReadyForProcessUartReceivedData())
	{
		resetError();
		//Process uart data
		UartParserResult uartParserResult = m_internalUartParser.parseInputMessage(m_pReceivedBuffer);
		setCurrentOperationType(m_internalUartParser.getIncomingOperationType());

		if(uartParserResult == UartParserResult::EmptyData)
		{
			//debug1();
			setError(static_cast<int8_t>(uartParserResult));
			setReadyForProcessUartReceivedData(false);
			setReadyForProcessUartResponseData(true);
			return;
		}
		else if(uartParserResult != UartParserResult::Ok)
		{
			//debug1();
			setError(static_cast<int8_t>(uartParserResult));
			setReadyForProcessUartReceivedData(false);
			//setReadyForProcessUartResponseData(true);
			//debug();
			return;
		}

		//Reset flag
		setReadyForProcessUartReceivedData(false);
		setReadyForProcessUartParsedMessage(true);
	}

	if(isReadyForProcessUartParsedMessage())
	{
		CInternalUartParser::MessageType msgType = m_internalUartParser.getIncomingMessageType();
		//CInternalUartParser::OperationType operationType = m_internalUartParser.getIncomingOperationType();
		//debug();
		if(msgType == CInternalUartParser::MessageType::Request)
		{
			//Process requests
			processRequest();
		}
		else
		{

			setError(static_cast<int8_t>(ControllerResult::BadMessageType));
			setReadyForProcessUartParsedMessage(false);
			setReadyForProcessUartResponseData(true);
			return;
		}

		setReadyForProcessUartParsedMessage(false);
	}
//
	if(isWaitingForRadioResponse())
	{
		if(isReadyForProcessReceivedRadioData())
		{//debug();
			setReadyForProcessReceivedRadioData(false);
			setWaitingForRadioResponse(false);
			setReadyForProcessUartResponseData(true);
		}
		else if(isTimeout())
		{
			stopTimer();
			setError(static_cast<int8_t>(ControllerResult::Timeout));

			setWaitingForRadioResponse(false);
			setReadyForProcessUartResponseData(true);
		}
	}
//
//	//Last step - send Uart response
	if(isReadyForProcessUartResponseData())
	{
		//debug();
		if(isError())
		{
			char buff[4];
			itoa(getErrorNo(), buff, 10);
			m_internalUartParser.prepareOutputMessage(
					CInternalUartParser::MessageType::Response,
					getCurrentOperationType(),
					buff,
					CInternalUartParser::MessageValidate::Error);
		}
		else
		{
			CInternalUartParser::OperationType operationType = getCurrentOperationType();
			char *content = nullptr;
			if(operationType!= CInternalUartParser::OperationType::SetTransmitterAddress)
			{
				content = m_dataBuffer;
			}
			m_internalUartParser.prepareOutputMessage(
					CInternalUartParser::MessageType::Response,
					operationType,
					content);
		}
//
		//Send data
		CUart::getInstance()->puts(m_internalUartParser.getDataBuffer());			//Send response by UART
		CUart::getInstance()->puts("\n\r");				//Terminate response

		//clean();

		setReadyForProcessUartResponseData(false);
		//setBusy(false);
	}

}

void CControllerTerminal::processRequest()
{
	CInternalUartParser::OperationType operationType = m_internalUartParser.getIncomingOperationType();
	if(operationType == CInternalUartParser::OperationType::SetTransmitterAddress)
	{

		//m_dataBuffer = m_internalUartParser.getDataBuffer();
		strcpy(m_dataBuffer, m_internalUartParser.getDataBuffer());

		//CUart::getInstance()->puts(getBufferPtr());			//Send response by UART
		//CUart::getInstance()->puts("\n\r");				//Terminate response

		processSetTransmitterAddress();
		setReadyForProcessUartResponseData(true);
	}
	else if(operationType == CInternalUartParser::OperationType::SendDataToAir)
	{//debug();
		//char b[15];
		//strcpy(b, "blabla1");
		//CNrf::getInstance()->sendDataToAir(b);
		//CUart::getInstance()->puts(m_internalUartParser.getDataBuffer());			//Send response by UART
		//CUart::getInstance()->puts("\n\r");				//Terminate response

		//CUart::getInstance()->puts(m_internalUartParser.getDataBuffer());			//Send response by UART
		//CUart::getInstance()->puts("\n\r");				//Terminate response

		strcpy(m_dataBuffer, m_internalUartParser.getDataBuffer());
		processSendDataToAir();
		setWaitingForRadioResponse(true);
	}
}

void CControllerTerminal::processSetTransmitterAddress()
{
	uint8_t nAddress = atoi(m_dataBuffer);
	CNrf::getInstance()->setTransmitterAdres(nAddress);
	CNrf::getInstance()->setReciverAddres(RX_ADDR_P0, nAddress);
}

void CControllerTerminal::processSendDataToAir()
{
	CNrf::getInstance()->sendDataToAir(m_dataBuffer);
	startTimer();
}

//Error handling
void CControllerTerminal::resetError()
{
	m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);
}

void CControllerTerminal::setError(int8_t err)
{
	m_nErrorNo = err;
}

int8_t CControllerTerminal::getErrorNo()
{
	return m_nErrorNo;
}

bool CControllerTerminal::isError()
{
	return (m_nErrorNo != static_cast<int8_t>(ControllerResult::Ok)) ? true : false;
}

//Timer interface
void CControllerTerminal::incrementtimerTick()
{
	m_nTimerValue++;
}

uint8_t CControllerTerminal::getTimerHandle() const
{
	return m_sTimerHandle;
}

bool CControllerTerminal::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}

void CControllerTerminal::startTimer()
{
	resetTimerValue();
	CTimer2::getInstance()->Enable(m_sTimerHandle);
}

void CControllerTerminal::stopTimer()
{
	CTimer2::getInstance()->Disable(m_sTimerHandle);
}

void CControllerTerminal::resetTimerValue()
{
	m_nTimerValue = 0;
}

//Callbacks
void CControllerTerminal::uartCallback(char *data)
{//debug1();
	//Save data buffer
	m_pReceivedBuffer = data;;

	//Set flag
	setReadyForProcessUartReceivedData(true);
}

void CControllerTerminal::timerCallback()
{
	incrementtimerTick();
}

void CControllerTerminal::nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	strcpy(m_dataBuffer, static_cast<char*>(nRF_RX_buff));
	setReadyForProcessReceivedRadioData(true);
}

