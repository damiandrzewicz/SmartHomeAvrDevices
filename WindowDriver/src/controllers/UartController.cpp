/*
 * ServoModelController.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#include "../uart/uart.h"
#include "UartController.h"

CUartController::CUartController() {
	m_connectorUartParser.registerTokenParser(&m_tokenParser);
	m_windowUartParser.registerTokenParser(&m_tokenParser);

}

CUartController::~CUartController() {
	// TODO Auto-generated destructor stub
}


void CUartController::uartDataReady(char *pData)
{
	clearNotofications();
	m_bUartDataReady = true;
	m_pData = pData;
}

char *CUartController::getData()
{
	return m_pData;
}

void CUartController::clearNotofications()
{
	m_bUartDataReady = false;
}

void CUartController::setServoModel1(CServoModel *pModel)
{
	m_servoModelArr[0] = pModel;
	//m_servoModel1 = state;
}

void CUartController::setServoModel2(CServoModel *pModel)
{
	m_servoModelArr[1] = pModel;
	//m_servoModel2 = state;
}

void CUartController::loopEvent()
{
	if(m_bUartDataReady)
	{
		if(!m_connectorUartParser.parse(getData()))
		{
			m_connectorUartParser.createErrorMsg(
					static_cast<uint8_t>(CConnectorUartDataParser::Error::ParserError), m_cUartMessage);
				CUart::getInstance()->puts(m_cUartMessage);
				m_bUartDataReady = false;
				return;
		}
		//Get operation direction
		if(m_connectorUartParser.getOperationDirection() != CConnectorUartDataParser::OperationDirection::Request)
		{
			m_connectorUartParser.createErrorMsg(
					static_cast<uint8_t>(CConnectorUartDataParser::Error::WrongOperationDirection), m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
			return;
		}

		//Get operation name
		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
		if(m_connectorUartParser.getOperationName() == CConnectorUartDataParser::OperationName::SendDataToDevice)
		{
			processModelMessage();
			m_connectorUartParser.createMessage(
					m_connectorUartParser.getOperationName(),
					CConnectorUartDataParser::OperationDirection::Response,
					m_cModelMessage,
					m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
		}
		else if(m_connectorUartParser.getOperationName() == CConnectorUartDataParser::OperationName::NotSupported)
		{
			m_connectorUartParser.createErrorMsg(
					static_cast<uint8_t>(CConnectorUartDataParser::Error::WrongOperationName), m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
			return;
		}
		m_bUartDataReady = false;
	}
}

bool CUartController::processModelMessage()
{
	//Temporary copy context to uardBuffer
	strcpy(m_cUartMessage, m_connectorUartParser.getContext());

	if(!m_windowUartParser.parse(m_cUartMessage))
	{
		m_windowUartParser.createErrorMsg(
				static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
		return false;
	}

	//Get operation direction
	if(m_windowUartParser.getOperationDirection() != CWindowUartDataParser::OperationDirection::Request)
	{
		m_windowUartParser.createErrorMsg(
				static_cast<uint8_t>(CWindowUartDataParser::Error::WrongOperationDirection), m_cModelMessage);
		return false;
	}

	char cContext[15];

	//Check if operation name is allowed
	if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetBlindMetadata)
	{
		CBlindMetadata blindMetadata;
		if(!m_windowUartParser.parseSetBlindMetadata(blindMetadata))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!processBlindMetadata(blindMetadata))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}

		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetBlindMetadata)
	{
		CBlindMetadata blindMetadata;
		if(!m_windowUartParser.parseGetBlindMetadata(blindMetadata))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!readBlindMetadata(blindMetadata))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}

		if(!m_windowUartParser.createGetBlindMetadataContext(blindMetadata, cContext))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::ManualDrive)
	{
		CBlindManualDrive manualDrive;
		if(!m_windowUartParser.parseManualDrive(manualDrive))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!processManualDrive(manualDrive))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}

		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetBlindState)
	{
		CBlindState blindState;
		if(!m_windowUartParser.parseGetBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!readBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}

		if(!m_windowUartParser.createGetBlindStateContext(blindState, cContext))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetBlindState)
	{
		CBlindState blindState;
		if(!m_windowUartParser.parseSetBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!processBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetCalibrateStep)
	{
		CBlindCalibrate blindCalibrate;
		if(!m_windowUartParser.parseSetCalibrate(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!processSetCalibrate(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::NotSupported)
	{
		m_windowUartParser.createErrorMsg(
				static_cast<uint8_t>(CWindowUartDataParser::Error::WrongOperationName), m_cModelMessage);
		return false;
	}

	//Prepare final message
	m_windowUartParser.createMessage(
			m_windowUartParser.getOperationName(),
			CWindowUartDataParser::OperationDirection::Response,
			cContext,
			m_cModelMessage);

	return true;
}

bool CUartController::readBlindMetadata(CBlindMetadata &refBlindMetadata)
{
	uint8_t nPos = refBlindMetadata.getBlindNumber() - 1;
	refBlindMetadata.setBlindType(m_servoModelArr[nPos]->getBlindType());
	refBlindMetadata.setBlindVisibility(m_servoModelArr[nPos]->getBlindVisibility());
	return true;
}

bool CUartController::processBlindMetadata(CBlindMetadata &refBlindMetadata)
{
	uint8_t nPos = refBlindMetadata.getBlindNumber() - 1;
	m_servoModelArr[nPos]->setBlindType(refBlindMetadata.getBlindType());
	m_servoModelArr[nPos]->setBlindVisibility(refBlindMetadata.getBlindVisibility());
	return true;
}

bool CUartController::processManualDrive(CBlindManualDrive &refManualDrive)
{
	//TODO
	return false;
}

bool CUartController::readBlindState(CBlindState &refBlindState)
{
	uint8_t nPos = refBlindState.getBlindNumber() - 1;
	refBlindState.setOpenPercent(m_servoModelArr[nPos]->getOpenPercent());
	refBlindState.setOpenSpeed(m_servoModelArr[nPos]->getOpenSpeed());
	refBlindState.setIsWindowClosed(m_servoModelArr[nPos]->isWindowClosed());
	return true;
}

bool CUartController::processBlindState(CBlindState &refBlindState)
{
	uint8_t nPos = refBlindState.getBlindNumber() - 1;
	m_servoModelArr[nPos]->setOpenPercent(refBlindState.getOpenPercent());
	m_servoModelArr[nPos]->setOpenSpeed(refBlindState.getOpenSpeed());
	return true;
}

bool CUartController::processSetCalibrate(CBlindCalibrate &refBlindCalibrate)
{
	uint8_t nPos = refBlindCalibrate.getBlindNumber() - 1;
	m_servoModelArr[nPos]->setCalibrateStep(refBlindCalibrate.getCalibrateStep());
	return true;
}

