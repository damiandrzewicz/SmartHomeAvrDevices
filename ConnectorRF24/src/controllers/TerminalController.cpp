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
		m_uartParser.parse(getData());

		//Get operation direction
		CUartDataParser::OperationDirection dir = m_uartParser.getOperationDirection();
		if(dir != CUartDataParser::OperationDirection::Request)
		{
			return;
		}

		//Get operation name
		CUartDataParser::OperationName opName = m_uartParser.getOperationName();
		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(opName));
		if(opName == CUartDataParser::OperationName::SayHello)
		{

		}
		else if(opName == CUartDataParser::OperationName::SetConnectorAddress)
		{

		}
		else if(opName == CUartDataParser::OperationName::GetConnectorAddress)
		{

		}
		else if(opName == CUartDataParser::OperationName::RestartConnector)
		{

		}
		else if(opName == CUartDataParser::OperationName::SendDataToDevice)
		{
			m_radioParser.createMessage(CRadioDataParser::OperationName::PassDataByAir,
					CRadioDataParser::OperationDirection::Request,
					m_uartParser.getContext(),
					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);
		}
		else if(opName == CUartDataParser::OperationName::NotSupported)
		{

		}
		//Clear flag
		m_bUartDataReady = false;
		m_bWaitingWorResponse = true;
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
			CUart::getInstance()->puts("timeout\r\n");
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

