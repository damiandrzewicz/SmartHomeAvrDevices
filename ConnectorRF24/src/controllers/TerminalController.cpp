/*
 * TerminalController.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: damian
 */

#include "TerminalController.h"
#include "../uart/uart.h"
#include "../utils/utils.h"
#include "../RF24/RF24.h"
#include "../settings.h"

CTerminalController::CTerminalController() {
	m_nTimeoutValue = 500;		//timeout value

}

CTerminalController::~CTerminalController() {
	// TODO Auto-generated destructor stub
}

//Override most important function
void CTerminalController::eventLoop()
{
	//Check if something received from UART
	if(m_bUartDataReady)
	{

		//Parse data
		if(!m_uartParser.parse(getData()))
		{
			m_uartParser.createErrorMsg(CUartDataParser::Error::ParserError, m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
			m_bUartDataReady = false;
			return;
		}

		//Get operation direction
		CUartDataParser::OperationDirection dir = m_uartParser.getOperationDirection();
		if(dir != CUartDataParser::OperationDirection::Request)
		{
			m_bUartDataReady = false;
			return;
		}

		//Get operation name
		m_operationName = m_uartParser.getOperationName();
		CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
		if(m_operationName == CUartDataParser::OperationName::SayHello)
		{
			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
			m_uartParser.createMessage(
					m_operationName,
					CUartDataParser::OperationDirection::Response,
					(char*)pgm_read_word( &DeviceStaticSettings[0] ),
					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		else if(m_operationName == CUartDataParser::OperationName::SetConnectorAddress)
		{
			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
			m_uartParser.createMessage(
					m_operationName,
					CUartDataParser::OperationDirection::Response,
					"ok",
					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		else if(m_operationName == CUartDataParser::OperationName::GetConnectorAddress)
		{
			char temp[6];
			itoa(RF24::getInstance()->getDeviceAddressAsInt(), temp, 10);
			m_uartParser.createMessage(
					m_operationName,
					CUartDataParser::OperationDirection::Response,
					temp,
					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		else if(m_operationName == CUartDataParser::OperationName::RestartConnector)
		{
			//TODO implement restarting connector
		}
		else if(m_operationName == CUartDataParser::OperationName::SendDataToDevice)
		{
			CUart::getInstance()->puts("hh");
			m_radioParser.createMessage(
					CRadioDataParser::OperationName::PassDataByAir,
					CRadioDataParser::OperationDirection::Request,
					m_uartParser.getContext(),
					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);

			m_bWaitingWorResponse = true;
		}
		else if(m_operationName == CUartDataParser::OperationName::NotSupported)
		{

		}
		//Clear flag
		m_bUartDataReady = false;
	}

	if(m_bWaitingWorResponse)
	{
		//Check if data recevied
		if(m_bRadioDataReady)
		{
			//Clear flags
			m_bWaitingWorResponse = false;
			m_bRadioDataReady = false;
			m_bProcessResponseStep = true;
		}
		else
		{
			//Check timeout
			if(!m_lTime)
			{CUart::getInstance()->puts("here\r\n");
				m_lTime = millis();
			}

			else if( (millis () - m_lTime) > m_nTimeoutValue)
			{
				m_bWaitingWorResponse = false;
				m_lTime = 0;
				m_bTimeout = true;
				m_bProcessResponseStep = true;
			}
		}
	}

	if(m_bProcessResponseStep)
	{
		if(m_bTimeout)
		{
			m_uartParser.createErrorMsg(CUartDataParser::Error::Timeout, m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		else
		{

			CUart::getInstance()->puts(m_pData);
		}
		//Clear flag
		m_bProcessResponseStep = false;
		m_bTimeout = false;
	}
}

