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

void CUartController::setServoModel1(ServoModel *state)
{
	m_servoModelArr[0] = state;
	//m_servoModel1 = state;
}

void CUartController::setServoModel2(ServoModel *state)
{
	m_servoModelArr[1] = state;
	//m_servoModel2 = state;
}

void CUartController::loopEvent()
{
	if(m_bUartDataReady)
	{
		if(!m_connectorUartParser.parse(getData()))
		{
			m_connectorUartParser.createErrorMsg(CConnectorUartDataParser::Error::ParserError, m_cUartMessage);
				CUart::getInstance()->puts(m_cUartMessage);
				m_bUartDataReady = false;
				return;
		}
		//Get operation direction
		if(m_connectorUartParser.getOperationDirection() != CConnectorUartDataParser::OperationDirection::Request)
		{
			m_connectorUartParser.createErrorMsg(CConnectorUartDataParser::Error::WrongOperationDirection, m_cUartMessage);
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
			m_connectorUartParser.createErrorMsg(CConnectorUartDataParser::Error::WrongOperationName, m_cUartMessage);
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
		m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
		return false;
	}

	//Get operation direction
	if(m_windowUartParser.getOperationDirection() != CWindowUartDataParser::OperationDirection::Request)
	{
		m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::WrongOperationDirection, m_cModelMessage);
		return false;
	}

	char cContext[15];

	//Check if operation name is allowed
	if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetBlindType)
	{
		SBlindType blindType;
		if(!m_windowUartParser.parseGetBlindType(blindType))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!readBlindType(blindType))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}

		if(!m_windowUartParser.createGetBlindTypeContext(blindType, cContext))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetBlindType)
	{
		SBlindType blindType;
		if(!m_windowUartParser.parseSetBlindType(blindType))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!processBlindType(blindType))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}

		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetState)
	{
		SBlindState blindState;
		if(!m_windowUartParser.parseGetBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!readBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}

		if(!m_windowUartParser.createGetBlindStateContext(blindState, cContext))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetState)
	{
		SBlindState blindState;
		if(!m_windowUartParser.parseSetBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!processBlindState(blindState))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}
		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::Calibrate)
	{
		SBlindCalibrate blindCalibrate;
		if(!m_windowUartParser.parseSetCalibrate(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!processSetCalibrate(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}
		strcpy(cContext, "ok");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::NotSupported)
	{
		m_windowUartParser.createErrorMsg(CWindowUartDataParser::Error::WrongOperationName, m_cModelMessage);
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

bool CUartController::readBlindType(SBlindType &refBlindType)
{
	uint8_t nPos = refBlindType.nBlindNo - 1;
	refBlindType.blindType = m_servoModelArr[nPos]->getBlindType();
	return true;
}

bool CUartController::processBlindType(SBlindType &refBlindType)
{
	uint8_t nPos = refBlindType.nBlindNo - 1;
	m_servoModelArr[nPos]->setBlindType(refBlindType.blindType);
	return true;
}

bool CUartController::readBlindState(SBlindState &refBlindState)
{
	uint8_t nPos = refBlindState.nBlindNo - 1;
	refBlindState.nOpenPercent = m_servoModelArr[nPos]->getOpenPercent();
	refBlindState.direction = m_servoModelArr[nPos]->getDirection();
	refBlindState.visibility = m_servoModelArr[nPos]->getVisibility();
	refBlindState.bIsWindowClosed = m_servoModelArr[nPos]->isWindowClosed();
	return true;
}

bool CUartController::processBlindState(SBlindState &refBlindState)
{
	uint8_t nPos = refBlindState.nBlindNo - 1;
	m_servoModelArr[nPos]->setOpenPercent(refBlindState.nOpenPercent);
	m_servoModelArr[nPos]->setDirection(refBlindState.direction);
	m_servoModelArr[nPos]->setVisibility(refBlindState.visibility);
	return true;
}

bool CUartController::processSetCalibrate(SBlindCalibrate &refBlindCalibrate)
{
	uint8_t nPos = refBlindCalibrate.nBlindNo - 1;
	m_servoModelArr[nPos]->setCalibrationStep(refBlindCalibrate.nCalibrateStep);
	return true;
}

