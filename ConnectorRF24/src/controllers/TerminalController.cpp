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
		if(!parseUartMessageWrapper(
				CUartDataParser::OperationDirection::Request,
				CRadioDataParser::OperationDirection::Request,
				m_uartOperationName))
		{
			CUart::getInstance()->puts(m_cMessage);
		}
		else
		{
			if(m_uartOperationName == CUartDataParser::OperationName::SendDataToDevice)
			{
				RF24::getInstance()->sendDataToAir(m_cMessage);
				m_bWaitingForResponse = true;

				//CUart::getInstance()->puts("sending uart:");
				//CUart::getInstance()->puts(m_cMessage);
				//CUart::getInstance()->puts("\r\n");
			}
			else
			{
				CUart::getInstance()->puts(m_cMessage);
			}
		}

//		if(parseUartMessageWrapper(CUartDataParser::OperationDirection::Request, m_uartOperationName))
//		{
//			if(m_uartOperationName == CUartDataParser::OperationName::SendDataToDevice)
//			{
//				RF24::getInstance()->sendDataToAir(m_cMessage);
//				m_bWaitingForResponse = true;
//			}
//			else
//			{
//				CUart::getInstance()->puts(m_cMessage);
//			}
//		}

		m_bUartDataReady = false;

//		//Parse data
//		if(!m_uartParser.parse(getData()))
//		{
//			//
//			m_uartParser.createErrorMsg(
//					static_cast<uint8_t>(CUartDataParser::Error::ParserError), m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//			m_bUartDataReady = false;
//			return;
//		}
//
//		//Get operation direction
//		CUartDataParser::OperationDirection dir = m_uartParser.getOperationDirection();
//		if(dir != CUartDataParser::OperationDirection::Request)
//		{
//			m_uartParser.createErrorMsg(
//					static_cast<uint8_t>(CUartDataParser::Error::WrongOperationDirection), m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//			m_bUartDataReady = false;
//			return;
//		}
//
//		//Get operation name
//		m_uartOperationName = m_uartParser.getOperationName();
//		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
//		if(m_uartOperationName == CUartDataParser::OperationName::SayHello)
//		{
//			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
//			m_uartParser.createMessage(
//					m_uartOperationName,
//					CUartDataParser::OperationDirection::Response,
//					(char*)pgm_read_word( &DeviceStaticSettings[0] ),
//					m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//		}
//		else if(m_uartOperationName == CUartDataParser::OperationName::SetConnectorAddress)
//		{
//			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
//			m_uartParser.createMessage(
//					m_uartOperationName,
//					CUartDataParser::OperationDirection::Response,
//					m_uartParser.getAdditionalText(CUartDataParser::AdditionalTexts::OkResponse),
//					m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//		}
//		else if(m_uartOperationName == CUartDataParser::OperationName::GetConnectorAddress)
//		{
//			char temp[6];
//			itoa(RF24::getInstance()->getDeviceAddressAsInt(), temp, 10);
//			m_uartParser.createMessage(
//					m_uartOperationName,
//					CUartDataParser::OperationDirection::Response,
//					temp,
//					m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//		}
//		else if(m_uartOperationName == CUartDataParser::OperationName::RestartConnector)
//		{
//			//TODO implement restarting connector
//		}
//		else if(m_uartOperationName == CUartDataParser::OperationName::SendDataToDevice)
//		{
//			//CUart::getInstance()->puts("hh1\r\n");
//			m_radioParser.createMessage(
//					CRadioDataParser::OperationName::PassDataByAir,
//					CRadioDataParser::OperationDirection::Request,
//					m_uartParser.getContext(),
//					m_cMessage);
//			//CUart::getInstance()->puts(m_cMessage);
//			RF24::getInstance()->sendDataToAir(m_cMessage);
//
//			m_bWaitingForResponse = true;
//			//CUart::getInstance()->puts("hh2\r\n");
//		}
//		else if(m_uartOperationName == CUartDataParser::OperationName::NotSupported)
//		{
//			m_uartParser.createErrorMsg(
//					static_cast<uint8_t>(CUartDataParser::Error::WrongOperationName), m_cMessage);
//			CUart::getInstance()->puts(m_cMessage);
//			m_bUartDataReady = false;
//			return;
//		}
//		//Clear flag
//		m_bUartDataReady = false;
	}

	if(m_bWaitingForResponse)
	{
		//Check if data recevied
		if(m_bRadioDataReady)
		{//CUart::getInstance()->puts("radiodatareadytimer\r\n");
			//Clear flags
			m_bWaitingForResponse = false;
			m_bRadioDataReady = false;
			m_bProcessResponseStep = true;
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

				m_bWaitingForResponse = false;
				m_bTimeout = true;
				m_bProcessResponseStep = true;
			}
		}
	}

	if(m_bProcessResponseStep)
	{
		if(m_bTimeout)
		{//CUart::getInstance()->puts("hh3\r\n");
			m_uartParser.createErrorMsg(
					static_cast<uint8_t>(CUartDataParser::Error::Timeout), m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		else
		{
			parseRadioMessageWrapper(
					CRadioDataParser::OperationDirection::Response,
					CUartDataParser::OperationDirection::Response,
					m_radioOperationName);

//			m_uartParser.createMessage(
//					m_uartOperationName,
//					CUartDataParser::OperationDirection::Response,
//					m_pData,
//					m_cMessage);
			CUart::getInstance()->puts(m_cMessage);
		}
		//Clear flag
		m_bProcessResponseStep = false;
		m_bTimeout = false;
		m_lTime = 0;

	}
}


