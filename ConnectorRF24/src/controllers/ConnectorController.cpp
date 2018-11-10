/*
 * ConnectorController.cpp
 *
 *  Created on: Aug 28, 2018
 *      Author: damian
 */

#include "ConnectorController.h"
#include "../uart/uart.h"
#include "../RF24/nRF24L01.h"
#include "../RF24/RF24.h"
#include "../utils/utils.h"

CConnectorController::CConnectorController() {
	m_nTimeoutValue = 100;		//timeout value

}

CConnectorController::~CConnectorController() {
	// TODO Auto-generated destructor stub
}

void CConnectorController::eventLoop()
{
	//Process messaged from Connector to Device
	if(m_bRadioDataReady && !m_bWaitingForUartResponse)
	{

		if(!parseRadioMessageWrapper(
				CRadioDataParser::OperationDirection::Request,
				CUartDataParser::OperationDirection::Request,
				m_radioOperationName))
		{
			//RF24::getInstance()->sendDataToAir("here");
			RF24::getInstance()->sendDataToAir(m_cMessage);
		}
		else
		{
			if(m_radioOperationName == CRadioDataParser::OperationName::PassDataByAir)
			{
				CUart::getInstance()->puts(m_cMessage);
				m_bWaitingForUartResponse = true;
			}
			else
			{
				RF24::getInstance()->sendDataToAir(m_cMessage);
			}
		}

		m_bRadioDataReady = false;
	}


	if(m_bWaitingForUartResponse)
	{
		//Check if data recevied
		if(m_bUartDataReady)
		{//CUart::getInstance()->puts("radiodatareadytimer\r\n");
			//Clear flags
			m_bWaitingForUartResponse = false;
			m_bUartDataReady = false;
			m_bProcessRadioResponseStep = true;
		}
		else
		{
			//CUart::getInstance()->putint(m_lTime, 10);
			//CUart::getInstance()->puts("\r\n");
			//Check timeout
			if(!m_lTime)
			{//CUart::getInstance()->puts("timo=0\r\n");
				m_lTime = millis();
			}

			else if( (millis () - m_lTime) > m_nTimeoutValue)
			{
				m_bWaitingForUartResponse = false;
				m_bTimeout = true;
				m_bProcessRadioResponseStep = true;
			}
		}
	}

	if(m_bProcessRadioResponseStep)
	{
		if(m_bTimeout)
		{//CUart::getInstance()->puts("hh3\r\n");
			m_radioParser.createErrorMsg(
					static_cast<uint8_t>(CRadioDataParser::Error::Timeout), m_cMessage);
		}
		else
		{
			parseUartMessageWrapper(
					CUartDataParser::OperationDirection::Response,
					CRadioDataParser::OperationDirection::Response,
					m_uartOperationName);
		}

		RF24::getInstance()->sendDataToAir(m_cMessage);

		//CUart::getInstance()->puts("Data:");
		//CUart::getInstance()->puts(m_cMessage);
		//CUart::getInstance()->puts("\n");


		//Clear flag
		m_bProcessRadioResponseStep = false;
		m_bTimeout = false;
		m_lTime = 0;
	}

	//Requests from device
	//Check if something received from UART
	if(m_bUartDataReady)
	{
		parseUartMessageWrapper(
				CUartDataParser::OperationDirection::Request,
				CRadioDataParser::OperationDirection::NotSupported,
				m_uartOperationName);
		CUart::getInstance()->puts(m_cMessage);
		m_bUartDataReady = false;	//Clear flag
	}
}

