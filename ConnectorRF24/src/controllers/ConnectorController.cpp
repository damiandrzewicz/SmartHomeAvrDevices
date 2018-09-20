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
		//Parse data
		if(!m_radioParser.parse(getData()))
		{
			m_radioParser.createErrorMsg(CRadioDataParser::Error::ParserError, m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);
			m_bRadioDataReady = false;
			return;
		}

		//Get operation direction
		CRadioDataParser::OperationDirection dir = m_radioParser.getOperationDirection();
		if(dir != CRadioDataParser::OperationDirection::Request)
		{
			m_radioParser.createErrorMsg(CRadioDataParser::Error::WrongOperationDirection, m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);
			m_bRadioDataReady = false;
			return;
		}
		CUart::getInstance()->puts(m_radioParser.getOperationDirectonText(dir));
		CUart::getInstance()->puts("\r\n");

		//Get operation name
		m_radioOperationName = m_radioParser.getOperationName();
		CUart::getInstance()->puts(m_radioParser.getOperationNameText(m_radioOperationName));
		CUart::getInstance()->puts("\r\n");
		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
		if(m_radioOperationName == CRadioDataParser::OperationName::PassDataByAir)
		{
			CUart::getInstance()->puts("processing\r\n");
//			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
//			m_radioParser.createMessage(
//					m_operationName,
//					CUartDataParser::OperationDirection::Response,
//					(char*)pgm_read_word( &DeviceStaticSettings[0] ),
//					m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);

			m_bWaitingForUartResponse = true;
		}
		else if(m_radioOperationName == CRadioDataParser::OperationName::NotSupported)
		{
			CUart::getInstance()->puts("notsuppo\r\n");
			m_radioParser.createErrorMsg(CRadioDataParser::Error::WrongOperationName, m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);
			m_bRadioDataReady = false;
			return;
		}
		//Clear flag
		m_bRadioDataReady = false;
	}

	//Process messages from Device to Connector
	//if()

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
		{CUart::getInstance()->puts("hh3\r\n");
			m_radioParser.createErrorMsg(CRadioDataParser::Error::Timeout, m_cMessage);
			RF24::getInstance()->sendDataToAir(m_cMessage);
		}
		else
		{
			RF24::getInstance()->sendDataToAir(m_pData);
		}
		//Clear flag
		m_bProcessRadioResponseStep = false;
		m_bTimeout = false;
		m_lTime = 0;
	}


//	if(m_bWaitingWorResponse)
//	{
//		//Check if data recevied
//		if(m_bRadioDataReady)
//		{//CUart::getInstance()->puts("radiodatareadytimer\r\n");
//			//Clear flags
//			m_bWaitingWorResponse = false;
//			m_bRadioDataReady = false;
//			m_bProcessResponseStep = true;
//		}
//		else
//		{
//			//CUart::getInstance()->putint(m_lTime, 10);
//			//CUart::getInstance()->puts("\r\n");
//			//Check timeout
//			if(!m_lTime)
//			{//CUart::getInstance()->puts("timo=0\r\n");
//				m_lTime = millis();
//			}
//
//			else if( (millis () - m_lTime) > m_nTimeoutValue)
//			{
//				m_bWaitingWorResponse = false;
//				m_bTimeout = true;
//				m_bProcessResponseStep = true;
//			}
//		}
//	}
//
//	if(m_bProcessResponseStep)
//	{
//		if(m_bTimeout)
//		{//CUart::getInstance()->puts("hh3\r\n");
//			m_uartParser.createErrorMsg(CUartDataParser::Error::Timeout, m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//		}
//		else
//		{
//			CUart::getInstance()->puts(m_pData);
//		}
//		//Clear flag
//		m_bProcessResponseStep = false;
//		m_bTimeout = false;
//		m_lTime = 0;
//	}
//
//
//
//	if(m_bRadioDataReady)
//	{
//		CUart::getInstance()->puts(getData());
//		RF24::getInstance()->sendDataToAir("omething responsed");
//		m_bRadioDataReady = false;
//	}
}

