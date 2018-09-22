/*
 * ServoModelController.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#include "ServoModelController.h"
#include "../uart/uart.h"

CServoModelController::CServoModelController() {
	m_uartParser.registerTokenParser(&m_tokenParser);
	m_modelParser.registerTokenParser(&m_tokenParser);

}

CServoModelController::~CServoModelController() {
	// TODO Auto-generated destructor stub
}


void CServoModelController::uartDataReady(char *pData)
{
	clearNotofications();
	m_bUartDataReady = true;
	m_pData = pData;
}

char *CServoModelController::getData()
{
	return m_pData;
}

void CServoModelController::clearNotofications()
{
	m_bUartDataReady = false;
}

void CServoModelController::setServoModel1(ServoModel *state)
{
	m_servoModelArr[0] = state;
	//m_servoModel1 = state;
}

void CServoModelController::setServoModel2(ServoModel *state)
{
	m_servoModelArr[1] = state;
	//m_servoModel2 = state;
}

void CServoModelController::loopEvent()
{
	if(m_bUartDataReady)
	{
		if(!m_uartParser.parse(getData()))
		{
			m_uartParser.createErrorMsg(CUartDataParser::Error::ParserError, m_cUartMessage);
				CUart::getInstance()->puts(m_cUartMessage);
				m_bUartDataReady = false;
				return;
		}
		//Get operation direction
		if(m_uartParser.getOperationDirection() != CUartDataParser::OperationDirection::Request)
		{
			m_uartParser.createErrorMsg(CUartDataParser::Error::WrongOperationDirection, m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
			return;
		}

		//Get operation name
		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
		if(m_uartParser.getOperationName() == CUartDataParser::OperationName::SendDataToDevice)
		{
			processModelMessage();
			m_uartParser.createMessage(
					m_uartParser.getOperationName(),
					CUartDataParser::OperationDirection::Response,
					m_cModelMessage,
					m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
		}
		else if(m_uartParser.getOperationName() == CUartDataParser::OperationName::NotSupported)
		{
			m_uartParser.createErrorMsg(CUartDataParser::Error::WrongOperationName, m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
			return;
		}
		m_bUartDataReady = false;
	}
}

bool CServoModelController::processModelMessage()
{
	//Temporary copy context to uardBuffer
	strcpy(m_cUartMessage, m_uartParser.getContext());

	if(!m_modelParser.parse(m_cUartMessage))
	{
		m_modelParser.createErrorMsg(CModelDataParser::Error::ParserError, m_cModelMessage);
		return false;
	}

	//Get operation direction
	if(m_modelParser.getOperationDirection() != CModelDataParser::OperationDirection::Request)
	{
		m_modelParser.createErrorMsg(CModelDataParser::Error::WrongOperationDirection, m_cModelMessage);
		return false;
	}

	//Check if operation name is allowed
	if(m_modelParser.getOperationName() == CModelDataParser::OperationName::GetBlindType)
	{
		SBlindType blindType;
		if(!m_modelParser.parseGetBlindType(blindType))
		{CUart::getInstance()->puts("error\r\n");
			m_modelParser.createErrorMsg(CModelDataParser::Error::ParserError, m_cModelMessage);
			return false;
		}

		if(!readBlindType(blindType))
		{
			m_modelParser.createErrorMsg(CModelDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}

		char cContext[10];
		if(!m_modelParser.createGetBlindTypeContext(blindType, cContext))
		{
			m_modelParser.createErrorMsg(CModelDataParser::Error::ProcessingError, m_cModelMessage);
			return false;
		}

		m_modelParser.createMessage(
				m_modelParser.getOperationName(),
				CModelDataParser::OperationDirection::Response,
				cContext,
				m_cModelMessage);
	}
	else if(m_modelParser.getOperationName() == CModelDataParser::OperationName::SetBlindType)
	{
//		SBlindType blindType;
//		m_modelParser.parseSetBlindType(blindType);
//		processSetBlindType(blindType);
//
//		m_servoModelArr[blindType.nBlindNo]->setBlindType(blindType.blindType);
//		m_modelParser.createMessage(
//				m_modelParser.getOperationName(),
//				CModelDataParser::OperationDirection::Response,
//				"ok",
//				m_cModelMessage);
	}
	else if(m_modelParser.getOperationName() == CModelDataParser::OperationName::GetState)
	{
//		SBlindState blindState;
//		m_modelParser.parseGetBlindState(blindState);
//
//		blindState.direction = m_servoModelArr[blindState.nBlindNo]->getDirection();
//		blindState.nOpenPercent = m_servoModelArr[blindState.nBlindNo]->getOpenPercent();
//		blindState.visibility = m_servoModelArr[blindState.nBlindNo]->getVisibility();
//		blindState.bIsWindowClosed = m_servoModelArr[blindState.nBlindNo]->isWindowClosed();
//
//		m_modelParser.createGetBlindState(blindState);
//
//		char cContext[10];
//		m_modelParser.createGetBlindTypeContext(&blindType, cContext);

	}
	else if(m_modelParser.getOperationName() == CModelDataParser::OperationName::SetState)
	{

	}
	else if(m_modelParser.getOperationName() == CModelDataParser::OperationName::Calibrate)
	{

	}
	else if(m_modelParser.getOperationName() == CModelDataParser::OperationName::NotSupported)
	{
		m_modelParser.createErrorMsg(CModelDataParser::Error::WrongOperationName, m_cModelMessage);
		return false;
	}

	return true;
}

bool CServoModelController::readBlindType(SBlindType &refBlindType)
{
	refBlindType.blindType = m_servoModelArr[refBlindType.nBlindNo]->getBlindType();
	return true;
}

