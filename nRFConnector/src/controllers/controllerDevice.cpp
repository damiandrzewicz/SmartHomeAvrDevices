/*
 * controllerDevice.cpp
 *
 *  Created on: Jun 29, 2018
 *      Author: damian
 */

#include "controllerDevice.h"

#include <string.h>
#include "../timer/timer.h"
#include "../uart/uart.h"
#include "../nrf/nrf.h"
#include "../nrf/nRF24L01_memory_map.h"

CControllerDevice::CControllerDevice() {
	// TODO Auto-generated constructor stub

}

CControllerDevice::~CControllerDevice() {
	// TODO Auto-generated destructor stub
}

//******Main event******
void CControllerDevice::controllerEvent()
{
	//if(isCheckUartMessage())
	//{
		//UartParserResult uartParserResult = m_internalUartParser.parseInputMessage(m_pReceivedBuffer);

//		if(uartParserResult == UartParserResult::EmptyData)
//		{
//			//debug1();
//			setError(static_cast<int8_t>(uartParserResult));
//			setCheckUartMessage(false);
//			setReadyForProcessRadioResponse(true);
//			return;
//		}
//		else if(uartParserResult != UartParserResult::Ok)
//		{
//			//debug1();
//			setError(static_cast<int8_t>(uartParserResult));
//			setCheckUartMessage(false);
//			setReadyForProcessRadioResponse(true);
//			//debug();
//			return;
//		}
//
//		CInternalUartParser::MessageType msgType = m_internalUartParser.getIncomingMessageType();
//		CInternalUartParser::OperationType operationType = m_internalUartParser.getIncomingOperationType();
//		if(msgType == CInternalUartParser::MessageType::Request)
//		{
//			//m_internalUartParser.getIncomingMessageType();
//			return;
//		}
//		else if(msgType == CInternalUartParser::MessageType::Response)
//		{
//			if(operationType == getCurrentOperationType())
//			{
//				strcpy(m_dataBuffer, m_internalUartParser.getDataBuffer());
//				setReceivedUartData(true);
//			}
//		}

		//Reset flag
		//setCheckUartMessage(false);
		//setReadyForProcessRadioResponse(true);
	//}

	if(isReadyForProcessReceivedRadioData())
	{
		resetError();

		setCurrentOperationType(CInternalUartParser::OperationType::PassData);
		//Build UART message
		m_internalUartParser.prepareOutputMessage(
				CInternalUartParser::MessageType::Request,
				getCurrentOperationType(),
				m_dataBuffer);

		//Send uart data
		CUart::getInstance()->puts(m_internalUartParser.getDataBuffer());			//Send response by UART
		CUart::getInstance()->puts("\r");				//Terminate response

		setReadyForProcessReceivedRadioData(false);
		setWaitForUartData(true);
	}

	if(isWaitingForUartData())
	{
		if(isReceivedUartData())
		{
			setReceivedUartData(false);
			setWaitForUartData(false);
			setReadyForProcessRadioResponse(true);
		}
		else if(isTimeout())
		{
			stopTimer();
			setError(static_cast<int8_t>(ControllerResult::Timeout));
			setWaitForUartData(false);
			setReadyForProcessRadioResponse(true);
		}
	}
//
//	if(isReadyForProcessRadioResponse())
//	{
//		CNrf::getInstance()->sendDataToAir(m_dataBuffer);
//		setReceivedUartData(false);
//	}
}

//Error handling
void CControllerDevice::resetError()
{
	m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);
}

void CControllerDevice::setError(int8_t err)
{
	m_nErrorNo = err;
}

int8_t CControllerDevice::getErrorNo()
{
	return m_nErrorNo;
}

bool CControllerDevice::isError()
{
	return (m_nErrorNo != static_cast<int8_t>(ControllerResult::Ok)) ? true : false;
}

//Timer interface
void CControllerDevice::incrementtimerTick()
{
	m_nTimerValue++;
}

uint8_t CControllerDevice::getTimerHandle() const
{
	return m_sTimerHandle;
}

bool CControllerDevice::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}

void CControllerDevice::startTimer()
{
	resetTimerValue();
	CTimer2::getInstance()->Enable(m_sTimerHandle);
}

void CControllerDevice::stopTimer()
{
	CTimer2::getInstance()->Disable(m_sTimerHandle);
}

void CControllerDevice::resetTimerValue()
{
	m_nTimerValue = 0;
}

//Callbacks
void CControllerDevice::uartCallback(char *data)
{
	//strcpy(m_dataBuffer, data);
	m_pReceivedBuffer = data;
	setCheckUartMessage(true);
}

void CControllerDevice::timerCallback()
{
	incrementtimerTick();
}

void CControllerDevice::nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	//Copy received data to buffer
	//m_pReceivedBuffer = static_cast<char*>(nRF_RX_buff);
	strcpy(m_dataBuffer, static_cast<char*>(nRF_RX_buff));

	//strcpy(m_dataBuffer, static_cast<char*>(nRF_RX_buff));
	setReadyForProcessReceivedRadioData(true);
}
