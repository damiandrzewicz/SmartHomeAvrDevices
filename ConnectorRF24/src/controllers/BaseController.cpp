/*
 * BaseController.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#include <stdlib.h>
#include "BaseController.h"


CBaseController::CBaseController() {
	m_radioParser.registerTokenParser(&m_tokenParser);
	m_uartParser.registerTokenParser(&m_tokenParser);
}

CBaseController::~CBaseController() {
	// TODO Auto-generated destructor stub
}

//Notofications
void CBaseController::radioDataReady(char *pData)
{
	clearNotofications();
	m_bRadioDataReady = true;
	m_pData = pData;
}

void CBaseController::uartDataReady(char *pData)
{
	clearNotofications();
	m_bUartDataReady = true;
	m_pData = pData;
}

char *CBaseController::getData()
{
	return m_pData;
}

void CBaseController::clearNotofications()
{
	m_bRadioDataReady = false;
	m_bUartDataReady = false;
}

bool CBaseController::parseUartMessageWrapper(
		CUartDataParser::OperationDirection expectedDir,
		CRadioDataParser::OperationDirection radioDir,
		CUartDataParser::OperationName &operationName)
{
	//Parse data
	if(!m_uartParser.parse(getData()))
	{
		m_uartParser.createErrorMsg(
				static_cast<uint8_t>(CUartDataParser::Error::ParserError), m_cMessage);
		return false;
	}

	//Get operation direction
	CUartDataParser::OperationDirection dir = m_uartParser.getOperationDirection();
	if(dir != expectedDir)
	{
		m_uartParser.createErrorMsg(
				static_cast<uint8_t>(CUartDataParser::Error::WrongOperationDirection), m_cMessage);
		return false;
	}

	//Get operation name
	operationName = m_uartParser.getOperationName();
	if(operationName == CUartDataParser::OperationName::SayHello)
	{
		//TODO
//		m_uartParser.createMessage(
//				operationName,
//				CUartDataParser::OperationDirection::Response,
//				(char*)pgm_read_word( &DeviceStaticSettings[0] ),
//				m_cMessage);
	}
	else if(operationName == CUartDataParser::OperationName::SetConnectorAddress)
	{
		RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
		m_uartParser.createMessage(
				operationName,
				CUartDataParser::OperationDirection::Response,
				m_uartParser.getAdditionalText(CUartDataParser::AdditionalTexts::OkResponse),
				m_cMessage);
	}
	else if(operationName == CUartDataParser::OperationName::GetConnectorAddress)
	{
		char temp[6];
		itoa(RF24::getInstance()->getDeviceAddressAsInt(), temp, 10);
		m_uartParser.createMessage(
				operationName,
				CUartDataParser::OperationDirection::Response,
				temp,
				m_cMessage);
	}
	else if(operationName == CUartDataParser::OperationName::RestartConnector)
	{
		//TODO implement restarting connector
	}
	else if(operationName == CUartDataParser::OperationName::SendDataToDevice)
	{
		//CUart::getInstance()->puts("hh1\r\n");
		m_radioParser.createMessage(
				CRadioDataParser::OperationName::PassDataByAir,
				radioDir,
				m_uartParser.getContext(),
				m_cMessage);
		//CUart::getInstance()->puts(m_cMessage);
		//RF24::getInstance()->sendDataToAir(m_cMessage);

		//m_bWaitingForResponse = true;
		//CUart::getInstance()->puts("hh2\r\n");
	}
	else if(operationName == CUartDataParser::OperationName::NotSupported)
	{
		m_uartParser.createErrorMsg(
				static_cast<uint8_t>(CUartDataParser::Error::WrongOperationName), m_cMessage);
		//CUart::getInstance()->puts(m_cMessage);
		//m_bUartDataReady = false;
		return false;
	}

	return true;
}

bool CBaseController::parseRadioMessageWrapper(
		CRadioDataParser::OperationDirection expectedDir,
		CUartDataParser::OperationDirection uartDir,
		CRadioDataParser::OperationName &operationName)
{
	//Parse data
	if(!m_radioParser.parse(getData()))
	{
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::ParserError), m_cMessage);
		//RF24::getInstance()->sendDataToAir(m_cMessage);
		//m_bRadioDataReady = false;
		return false;
	}

	//Get operation direction
	CRadioDataParser::OperationDirection dir = m_radioParser.getOperationDirection();
	if(dir != expectedDir)
	{
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::WrongOperationDirection), m_cMessage);
		//RF24::getInstance()->sendDataToAir(m_cMessage);
		//m_bRadioDataReady = false;
		return false;
	}
	//CUart::getInstance()->puts(m_radioParser.getOperationDirectonText(dir));
	//CUart::getInstance()->puts("\r\n");

	//Get operation name
	operationName = m_radioParser.getOperationName();
	//CUart::getInstance()->puts(m_radioParser.getOperationNameText(m_radioOperationName));
	//CUart::getInstance()->puts("\r\n");
	//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
	if(operationName == CRadioDataParser::OperationName::PassDataByAir)
	{
		//CUart::getInstance()->puts("processing\r\n");
		m_uartParser.createMessage(
				CUartDataParser::OperationName::SendDataToDevice,
				/*CParserInterface::OperationDirection::Request*/uartDir,
				m_radioParser.getContext(),
				m_cMessage);
//			RF24::getInstance()->setDeviceAddress(atoi(m_uartParser.getContext()));
//			m_radioParser.createMessage(
//					m_operationName,
//					CUartDataParser::OperationDirection::Response,
//					(char*)pgm_read_word( &DeviceStaticSettings[0] ),
//					m_cMessage);

		//CUart::getInstance()->puts(m_cMessage);

		//m_bWaitingForUartResponse = true;
	}
	else if(operationName == CRadioDataParser::OperationName::NotSupported)
	{
		//CUart::getInstance()->puts("notsuppo\r\n");
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::WrongOperationName), m_cMessage);
		//RF24::getInstance()->sendDataToAir(m_cMessage);
		//m_bRadioDataReady = false;
		return false;
	}

	return true;
}
