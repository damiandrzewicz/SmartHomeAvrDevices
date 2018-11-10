/*
 * ServoModelController.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#include "../uart/uart.h"
#include "UartController.h"
#include "../settings.h"

//Per

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
	//initModelFromEeprom(pModel->getBlindNumber());

	//m_servoModel1 = state;
}

void CUartController::setServoModel2(CServoModel *pModel)
{
	m_servoModelArr[1] = pModel;
	//initModelFromEeprom(pModel->getBlindNumber());
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

		//Get operation name
		//CUart::getInstance()->puts(m_uartParser.getOperationNameText(m_operationName));
		CConnectorUartDataParser::OperationName operationName = m_connectorUartParser.getOperationName();
		CConnectorUartDataParser::OperationDirection operationDirection = m_connectorUartParser.getOperationDirection();

		if(operationName == CConnectorUartDataParser::OperationName::NotSupported)
		{
			m_connectorUartParser.createErrorMsg(
					static_cast<uint8_t>(CConnectorUartDataParser::Error::WrongOperationName), m_cUartMessage);
			CUart::getInstance()->puts(m_cUartMessage);
			m_bUartDataReady = false;
			return;
		}

		if(operationDirection == CConnectorUartDataParser::OperationDirection::Request)
		{
			if(operationName == CConnectorUartDataParser::OperationName::SendDataToDevice)
			{
				if(prepareMessage())
				{
					bool bOperationMatch;
					processDeviceMessage(bOperationMatch);
					if(!bOperationMatch)	processModelMessage(bOperationMatch);
				}





				m_connectorUartParser.createMessage(
						m_connectorUartParser.getOperationName(),
						CConnectorUartDataParser::OperationDirection::Response,
						m_cModelMessage,
						m_cUartMessage);
				CUart::getInstance()->puts(m_cUartMessage);
			}
		}
		else if(operationDirection == CConnectorUartDataParser::OperationDirection::Response)
		{
			const char *pOk = m_connectorUartParser.getAdditionalText(CParserInterface::AdditionalTexts::OkResponse);
			bool bResponse = false;
			bResponse = (strcmp(m_connectorUartParser.getContext(), pOk) == 0);

			if(operationName == CConnectorUartDataParser::OperationName::PowerUpConnector)
			{
				if(bResponse)
				{
					m_bConnectorPowerUp = true;
				}
			}
			else if(operationName == CConnectorUartDataParser::OperationName::PowerDownConnector)
			{
				if(bResponse)
				{
					m_bConnectorPowerUp = false;
					m_bConnectorAddressSet = false;
				}
			}
			else if(operationName == CConnectorUartDataParser::OperationName::SetConnectorAddress)
			{
				if(bResponse)
				{
					m_bConnectorAddressSet = true;
				}
			}
		}

		m_bUartDataReady = false;
	}

	initConnectorProcess();
}

void CUartController::setConnectorAddress(const char *pAddress)
{
	uint8_t nDeviceAddress = atoi(pAddress);
	char addressBuff[10];
	itoa(nDeviceAddress, addressBuff, 10);
	m_connectorUartParser.createMessage(
						CConnectorUartDataParser::OperationName::SetConnectorAddress,
						CConnectorUartDataParser::OperationDirection::Request,
						addressBuff,
						m_cUartMessage);
	CUart::getInstance()->puts(m_cUartMessage);
}

void CUartController::initConnectorProcess()
{
	if(!m_bConnectorPowerUp)
	{
		m_connectorUartParser.createMessage(
							CConnectorUartDataParser::OperationName::PowerUpConnector,
							CConnectorUartDataParser::OperationDirection::Request,
							m_connectorUartParser.getAdditionalText(CParserInterface::AdditionalTexts::EmptyContext),
							m_cUartMessage);
		CUart::getInstance()->puts(m_cUartMessage);
		m_bConnectorPowerUp = true;
	}
}

bool CUartController::prepareMessage()
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

	return true;
}

bool CUartController::processModelMessage(bool &bFoundMessage)
{
	bFoundMessage = false;
	char cContext[15];

	//Check if operation name is allowed
	if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetBlindMetadata)
	{
		bFoundMessage = true;

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

		strcpy(cContext, "$ok$");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetBlindMetadata)
	{
		bFoundMessage = true;

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
		bFoundMessage = true;

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

		strcpy(cContext, "$ok$");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetBlindState)
	{
		bFoundMessage = true;

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
		bFoundMessage = true;

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
		strcpy(cContext, "$ok$");
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SetCalibrateStep)
	{
		bFoundMessage = true;

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
		strcpy(cContext, "$ok$");

	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::GetCalibrateState)
	{
		bFoundMessage = true;

		CBlindCalibrate blindCalibrate;
		if(!m_windowUartParser.parseGetCalibrateState(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ParserError), m_cModelMessage);
			return false;
		}

		if(!readCalibrateState(blindCalibrate))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}

		if(!m_windowUartParser.createGetBlindCalibrateContext(blindCalibrate, cContext))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
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

bool CUartController::processDeviceMessage(bool &bFoundMessage)
{
	bFoundMessage = false;
	char cContext[25];

	//Check if operation name is allowed

	if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::SayHello)
	{
		bFoundMessage = true;
		if(!m_windowUartParser.createSayHelloContext(DEVICE_NAME, DEVICE_ADDRESS, cContext))
		{
			m_windowUartParser.createErrorMsg(
					static_cast<uint8_t>(CWindowUartDataParser::Error::ProcessingError), m_cModelMessage);
			return false;
		}
	}
	else if(m_windowUartParser.getOperationName() == CWindowUartDataParser::OperationName::RestartDevice)
	{
		bFoundMessage = true;
		strcpy(cContext, "$ok$");
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
	refBlindMetadata.getBlindMetadataObject().blindType = m_servoModelArr[nPos]->getBlindMetadataObject().blindType;
	refBlindMetadata.getBlindMetadataObject().visibility = m_servoModelArr[nPos]->getBlindMetadataObject().visibility;
	return true;
}

bool CUartController::processBlindMetadata(CBlindMetadata &refBlindMetadata)
{
	uint8_t nPos = refBlindMetadata.getBlindNumber() - 1;
	m_servoModelArr[nPos]->getBlindMetadataObject().blindType = refBlindMetadata.getBlindMetadataObject().blindType;
	m_servoModelArr[nPos]->getBlindMetadataObject().visibility = refBlindMetadata.getBlindMetadataObject().visibility;
	return true;
}

bool CUartController::processManualDrive(CBlindManualDrive &refManualDrive)
{
	uint8_t nPos = refManualDrive.getBlindNumber() - 1;
	m_servoModelArr[nPos]->setManualDriveDirection(refManualDrive.getManualDriveDirection());
	return true;
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

bool CUartController::readCalibrateState(CBlindCalibrate &refBlindCalibrate)
{
	uint8_t nPos = refBlindCalibrate.getBlindNumber() - 1;
	refBlindCalibrate.getCalibrateMetadataObject().m_nIsCalibrated = (m_servoModelArr[nPos]->getCalibrateMetadataObject().m_nIsCalibrated);
	return true;
}

bool CUartController::processSetCalibrate(CBlindCalibrate &refBlindCalibrate)
{
	uint8_t nPos = refBlindCalibrate.getBlindNumber() - 1;
	m_servoModelArr[nPos]->setCalibrateStep(refBlindCalibrate.getCalibrateStep());
	return true;
}

