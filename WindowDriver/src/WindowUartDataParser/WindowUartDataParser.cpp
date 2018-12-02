/*
 * ModelDataParser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */
#include <string.h>
#include "../uart/uart.h"
#include "TokenParser/TokenParser.h"
#include "WindowUartDataParser.h"

const char *const OpNameText[] PROGMEM = {
        "SBM",					//SetBlindMetadata,
        "GBM",					//GetBlindMetadata,
        "MD",					//ManualDrive,
        "SBS",					//SetBlindState,
        "GBS",					//GetBlindState,
        "SCS",					//SetCalibrateStep,
		"GCS",					//GetCalibrateState

		"RD",					//RestartDevice
		"SH",					//SayHello

        "no"					//NotSupported
};

CWindowUartDataParser::CWindowUartDataParser() {
	// TODO Auto-generated constructor stub

}

CWindowUartDataParser::~CWindowUartDataParser() {
	// TODO Auto-generated destructor stub
}

bool CWindowUartDataParser::parse(char *pData)
{
	if(!checkTokenParser())
			return false;
	//CUart::getInstance()->puts("t1 ");
	//CUart::getInstance()->puts(pData);
	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(pData, '!') != TokenParseResult::Ok)
	{
		return false;
	}
	//CUart::getInstance()->puts("t2");
	//Parse
	if(m_pTokenParser->parseData(pData, getAdditionalText(AdditionalTexts::ExclMark)) != TokenParseResult::Ok)
	{
		return false;
	}
	//CUart::getInstance()->puts("t3");
	//Get operation name
	OperationName opName = parseOperationName(m_pTokenParser->getNextToken());
	if(opName == OperationName::NotSupported)
		return false;

	setOperationName(opName);

	//Get operation direction
	OperationDirection opDir = CParserInterface::parseOperationDirection(m_pTokenParser->getNextToken());
	if(opDir == OperationDirection::NotSupported)
		return false;

	setOperationDirection(opDir);

	char *pContext = m_pTokenParser->getNextToken();
	if(pContext != nullptr && strcmp(pContext, PSTR("\0")))
		m_pContext = pContext;
	else
		return false;

	return true;
}

bool CWindowUartDataParser::createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult)
{
	if(!pResult)
		return false;

	//Prepare output message
	strcpy(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	strcat(pResult, getOperationNameText(opName));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	strcat(pResult, getOperationDirectonText(opDir));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));
	//strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, pContext);
	//strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, getAdditionalText(AdditionalTexts::ExclMark));

	return true;
}

CWindowUartDataParser::OperationName CWindowUartDataParser::getOperationName()
{
	return static_cast<OperationName>(CParserInterface::getOperationNameAsUint());
}

void CWindowUartDataParser::setOperationName(OperationName op)
{
	CParserInterface::setOperationName(static_cast<uint8_t>(op));
}

char *CWindowUartDataParser::getOperationNameText(OperationName op)
{
	return (char*)pgm_read_word( &OpNameText[static_cast<uint8_t>(op)] );
}

CWindowUartDataParser::OperationName CWindowUartDataParser::parseOperationName(char *pOperationName)
{
	if(!strcmp(pOperationName, getOperationNameText(OperationName::SetBlindMetadata)))
		return OperationName::SetBlindMetadata;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetBlindMetadata)))
		return OperationName::GetBlindMetadata;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::ManualDrive)))
		return OperationName::ManualDrive;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SetBlindState)))
		return OperationName::SetBlindState;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetBlindState)))
		return OperationName::GetBlindState;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SetCalibrateStep)))
		return OperationName::SetCalibrateStep;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::GetCalibrateState)))
		return OperationName::GetCalibrateState;

	else if(!strcmp(pOperationName, getOperationNameText(OperationName::SayHello)))
		return OperationName::SayHello;
	else if(!strcmp(pOperationName, getOperationNameText(OperationName::RestartDevice)))
		return OperationName::RestartDevice;

	else /*if(!strcmp(pOperationName, getOperationNameText(OperationName::NotSupported)))*/
		return OperationName::NotSupported;
}

bool CWindowUartDataParser::createErrorMsg(uint8_t errNo, char *pResult)
{
	return CParserInterface::createErrorMsg(errNo, AdditionalTexts::ExclMark, pResult);
}

bool CWindowUartDataParser::parseBlindNo(uint8_t &nBlindNo)
{
	strcpy(m_modelContextArr, m_pContext);

	if(!checkTokenParser())
			return false;

	//Check trimming chars
	if(m_pTokenParser->checkTrimmingChars(m_modelContextArr, '$') != TokenParseResult::Ok)
	{
		return false;
	}
	//Parse
	if(m_pTokenParser->parseData(m_modelContextArr, getAdditionalText(AdditionalTexts::Dollar)) != TokenParseResult::Ok)
	{
		return false;
	}

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;

	uint8_t blindNo = atoi(cTemp);

	if(blindNo == 1 || blindNo == 2)
	{
		nBlindNo = blindNo;	//Save data
		return true;
	}
	else
		return false;
}

bool CWindowUartDataParser::parseSetBlindMetadata(CBlindMetadata &refBlindMetadata)
{
	if(!parseBlindNo(refBlindMetadata.getBlindNumber()))
		return false;

	//CUart::getInstance()->putint(refBlindMetadata.getBlindNumber(), 10);

	//CUart::getInstance()->puts("here");

	//Get blind type
	char *cTemp = m_pTokenParser->getNextToken();
	//CUart::getInstance()->puts(cTemp);
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	//refBlindMetadata.setBlindType(static_cast<WindowData::BlindType>(atoi(cTemp)));
	refBlindMetadata.getBlindMetadataObject().blindType = static_cast<WindowData::BlindType>(atoi(cTemp));


	//Get blind visibility
	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	//refBlindMetadata.setBlindVisibility(static_cast<WindowData::Visibility>(atoi(cTemp)));
	refBlindMetadata.visibility = static_cast<WindowData::Visibility>(atoi(cTemp));

	//Get motor side
	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindMetadata.getBlindMetadataObject().motorSide = static_cast<WindowData::MotorSide>(atoi(cTemp));

	return true;
}

bool CWindowUartDataParser::parseGetBlindMetadata(CBlindMetadata &refBlindMetadata)
{
	if(!parseBlindNo(refBlindMetadata.getBlindNumber()))
		return false;

	return true;
}

bool CWindowUartDataParser::parseManualDrive(CBlindManualDrive &refManualDrive)
{
	if(!parseBlindNo(refManualDrive.getBlindNumber()))
		return false;

	//Get blind direction
	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refManualDrive.setManualDriveDirection( static_cast<WindowData::Direction>(atoi(cTemp)));

	return true;
}

bool CWindowUartDataParser::parseGetBlindState(CBlindState &refBlindState)
{
	if(!parseBlindNo(refBlindState.getBlindNumber()))
		return false;

	return true;
}

bool CWindowUartDataParser::parseSetBlindState(CBlindState &refBlindState)
{
	if(!parseBlindNo(refBlindState.getBlindNumber()))
		return false;

	//Set open percent
	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.setOpenPercent(static_cast<uint8_t>(atoi(cTemp)));

	//Set open speed
	cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindState.setOpenSpeed(static_cast<uint8_t>(atoi(cTemp)));

	return true;
}

bool CWindowUartDataParser::parseSetCalibrate(CBlindCalibrate &refBlindCalibrate)
{
	if(!parseBlindNo(refBlindCalibrate.getBlindNumber()))
		return false;

	char *cTemp = m_pTokenParser->getNextToken();
	if(cTemp == nullptr || !strcmp(cTemp, ""))
		return false;
	refBlindCalibrate.setCalibrateStep( static_cast<uint8_t>(atoi(cTemp)));

	return true;
}

bool CWindowUartDataParser::parseGetCalibrateState(CBlindCalibrate &refBlindCalibrate)
{
	if(!parseBlindNo(refBlindCalibrate.getBlindNumber()))
		return false;

	return true;
}

bool CWindowUartDataParser::createGetBlindMetadataContext(CBlindMetadata &refBlindMetadata, char *pResult)
{
	if(pResult == nullptr)
		return false;

	char cTemp[5];
	//itoa(static_cast<uint8_t>(refBlindMetadata.getBlindType()), cTemp, 10);
	itoa(static_cast<uint8_t>(refBlindMetadata.getBlindMetadataObject().blindType), cTemp, 10);
	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

//	CUart::getInstance()->puts(pResult);
//	CUart::getInstance()->puts("[1]\r\n");

	itoa(static_cast<uint8_t>(refBlindMetadata.visibility), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	itoa(static_cast<uint8_t>(refBlindMetadata.getBlindMetadataObject().motorSide), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

//	CUart::getInstance()->puts(pResult);
//	CUart::getInstance()->puts("[2]\r\n");

	//Isinitialized
//	itoa(static_cast<uint8_t>(refBlindMetadata.getBlindMetadataObject().isMetadataInitialized), cTemp, 10);
//	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
//	strcat(pResult, cTemp);


//	CUart::getInstance()->puts(pResult);
//	CUart::getInstance()->puts("[3]\r\n");

	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	return true;
}

bool CWindowUartDataParser::createGetBlindStateContext(CBlindState &refBlindState, char *pResult)
{
	if(pResult == nullptr)
		return false;

	char cTemp[4];

	//Open percent
	itoa(static_cast<uint8_t>(refBlindState.getOpenPercent().getValue()), cTemp, 10);
	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	//open speed
	itoa(static_cast<uint8_t>(refBlindState.getOpenSpeed()), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	////Is window closed
	itoa(static_cast<uint8_t>(refBlindState.isWindowClosed()), cTemp, 10);
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	return true;
}

bool CWindowUartDataParser::createGetBlindCalibrateContext(CBlindCalibrate &refBlindCalibrate, char *pResult)
{
	if(pResult == nullptr)
		return false;

	char cTemp[4];

	//IsCalibrated
	itoa(static_cast<uint8_t>(refBlindCalibrate.getCalibrateMetadataObject().bIsCalibrated), cTemp, 10);
	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, cTemp);

	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	return true;
}

bool CWindowUartDataParser::createSayHelloContext(
		const char *pName,
		const char *pAddress,
		char *pResult)
{
	if(pResult == nullptr)
		return false;

	//DeviceName
	strcpy(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, pName);

	//DeviceAddress
	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));
	strcat(pResult, pAddress);

	strcat(pResult, getAdditionalText(AdditionalTexts::Dollar));

	return true;
}

