/*
 * BaseController.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#include <stdlib.h>
#include <string.h>
#include "BaseController.h"

#include "../uart/uart.h"


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
	//memset(m_cMessage, 0, sizeof(m_cMessage));

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

	if(operationName == CUartDataParser::OperationName::SetConnectorAddress)
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
	else if(operationName == CUartDataParser::OperationName::SendDataToDevice)
	{
		if(m_bConnectorStarted)
		{
			m_radioParser.createMessage(
					CRadioDataParser::OperationName::PassDataByAir,
					radioDir,
					m_uartParser.getContext(),
					m_cMessage);
		}
		else
		{
			m_uartParser.createErrorMsg(
					static_cast<uint8_t>(CUartDataParser::ConnectorError::ConnectorNotStarted), m_cMessage);
			//CUart::getInstance()->puts(m_cMessage);
			//m_bUartDataReady = false;
			return false;
		}
	}
	else if(operationName == CUartDataParser::OperationName::PowerUpConnector)
	{
		RF24::getInstance()->realPowerDown();
		RF24::getInstance()->realPowerUp();
		m_uartParser.createMessage(
				operationName,
				CUartDataParser::OperationDirection::Response,
				m_uartParser.getAdditionalText(CUartDataParser::AdditionalTexts::OkResponse),
				m_cMessage);
		m_bConnectorStarted = true;
	}
	else if(operationName == CUartDataParser::OperationName::PowerDownConnector)
	{
		RF24::getInstance()->realPowerDown();
		m_uartParser.createMessage(
				operationName,
				CUartDataParser::OperationDirection::Response,
				m_uartParser.getAdditionalText(CUartDataParser::AdditionalTexts::OkResponse),
				m_cMessage);
		m_bConnectorStarted = false;
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
	//memset(m_cMessage, 0, sizeof(m_cMessage));

	//Parse data
	if(!m_radioParser.parse(getData()))
	{
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::ParserError), m_cMessage);
		return false;
	}

	//Get operation direction
	CRadioDataParser::OperationDirection dir = m_radioParser.getOperationDirection();
	if(dir != expectedDir)
	{
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::WrongOperationDirection), m_cMessage);

		return false;
	}

	//Get operation name
	operationName = m_radioParser.getOperationName();

	if(operationName == CRadioDataParser::OperationName::PassDataByAir)
	{
		m_uartParser.createMessage(
				CUartDataParser::OperationName::SendDataToDevice,
				/*CParserInterface::OperationDirection::Request*/uartDir,
				m_radioParser.getContext(),
				m_cMessage);
	}
	else if(operationName == CRadioDataParser::OperationName::NotSupported)
	{
		m_radioParser.createErrorMsg(
				static_cast<uint8_t>(CRadioDataParser::Error::WrongOperationName), m_cMessage);
		return false;
	}

	return true;
}
