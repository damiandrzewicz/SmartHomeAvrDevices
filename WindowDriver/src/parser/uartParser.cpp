/*
 * uartParser.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#include "uartParser.h"
#include "internalConnParser.h"
#include "../uart/uart.h"
#include "../../settings.h"

bool CUartParser::s_bIsCreated = 0;
CUartParser * CUartParser::s_instance = nullptr;

CUartParser::CUartParser() {
	// TODO Auto-generated constructor stub

}

CUartParser::~CUartParser() {
	// TODO Auto-generated destructor stub
}

void CUartParser::registerServo(CServo *srv1, CServo *srv2)
{
	m_pServo1 = srv1;
	m_pServo2 = srv2;
}

//Setters
void CUartParser::setReadyForProcessData(bool value)
{
	m_bReadyRofProcessData = value;
}

void CUartParser::setReadyForProcessResponse(bool value)
{
	m_breadyForProcessResponse = value;
}

//Getters
bool CUartParser::isReadyForProcessData()
{
	return m_bReadyRofProcessData;
}

bool CUartParser::isReadyForProcessResponse()
{
	return m_breadyForProcessResponse;
}

CUartParser * CUartParser::getInstance()
{
	if(!s_bIsCreated)
	{
		static CUartParser obj;
		s_instance = &obj;
		s_bIsCreated = 1;
	}
	return s_instance;
}

void CUartParser::setDataBuffer(char *buffer)
{
	m_buffer = buffer;
}

void CUartParser::setResponseBuffer(char *data)
{
	strcpy(m_responseBuffer, data);
}

void CUartParser::clearResponseBuffer()
{
	memset(m_responseBuffer, 0, sizeof m_responseBuffer);
}

void CUartParser::prepareCallbackMsg(char *buff)
{
	//char buff[10];
	strcpy(buff, "#resp#");

	if(m_bError)
		strcat(buff, "err#");
	else
	{
		strcat(buff, "ok");
		strcat(buff, m_responseBuffer);
		strcat(buff, "#");
	}

}

void CUartParser::setError(bool value)
{
	m_bError = value;
}

void CUartParser::processStepTransition(uint8_t step)
{
	if(step == 1)
		setReadyForProcessData(true);
	else
		setReadyForProcessData(false);

	if(step == 2)
		setReadyForProcessResponse(true);
	else
		setReadyForProcessResponse(false);
}

void CUartParser::clearStepTransition()
{
	setReadyForProcessData(false);
	setReadyForProcessResponse(false);
}

void CUartParser::sendCallbackMsg()
{
	CInternalConnParser internalConnParser;

	char responseBuff[30];
	prepareCallbackMsg(responseBuff);

	internalConnParser.prepareOutputMessage(responseBuff);

	CUart::getInstance()->puts(internalConnParser.getBuffer());
	CUart::getInstance()->puts("\r\n");
}

//Operation process
bool CUartParser::processSayHello(CDataParser *data)
{
	char buff[15];
	strcpy(buff, "(");
	strcat(buff, DEVICE_ADDRESS);
	strcat(buff, "!");
	char i[3];
	strcat(buff, itoa(BLIND_QUANTITY, i, 10));
	strcat(buff, ")");
	setResponseBuffer(buff);

	return true;
}

bool CUartParser::processGetBlindType(CDataParser *data)
{
	uint8_t nBlindNo = atoi(data->getNextToken());

	CUart::getInstance()->putint(nBlindNo, 10);
	CUart::getInstance()->puts("\r\n");


	CServo::BlindType blindType;
	if(nBlindNo == 1)
		blindType = m_pServo1->getblindType();
	else if(nBlindNo == 2)
		blindType = m_pServo2->getblindType();

	char temp[3];
	itoa(static_cast<uint8_t>(blindType), temp, 10);
	CUart::getInstance()->puts(temp);
	CUart::getInstance()->puts("\r\n");

	char buff[15];
	strcpy(buff, "(");
	strcat(buff, temp);
	strcat(buff, ")");
	setResponseBuffer(buff);

	return true;
}

bool CUartParser::processGetBlindState(CDataParser *data)
{
	uint8_t nBlindNo = atoi(data->getNextToken());

	uint8_t openPercent = 0;
	CServo::Visibility visibility = CServo::Visibility::None;
	bool isWindowClosed = false;
	if(nBlindNo == 1)
	{
		visibility = m_pServo1->getVisibility();
		openPercent = m_pServo1->getOpenPercent();
		isWindowClosed = m_pServo1->isWindowClosed();
	}
	else if(nBlindNo == 2)
	{
		visibility = m_pServo2->getVisibility();
		openPercent = m_pServo2->getOpenPercent();
		isWindowClosed = m_pServo2->isWindowClosed();
	}

	char buff[15];
	char temp[5];
	strcpy(buff, "(");
	strcat(buff, itoa(openPercent, temp, 10));
	strcat(buff, "!");
	strcat(buff, itoa(static_cast<uint8_t>(visibility), temp, 10));
	strcat(buff, "!");
	strcat(buff, itoa(static_cast<uint8_t>(isWindowClosed), temp, 10));
	strcat(buff, ")");
	setResponseBuffer(buff);

	return true;
}

bool CUartParser::processSetblindState(CDataParser *data)
{
	uint8_t nBlindNo = atoi(data->getNextToken());

	//Get direction
	uint8_t nDirection = atoi(data->getNextToken());

	//Get open percent
	uint8_t nOpenPercent = atoi(data->getNextToken());

	//Get speed percent
	uint8_t nSpeedPercent = atoi(data->getNextToken());

	//Get autoDrive
	uint8_t nAutoDrive = atoi(data->getNextToken());

	//Get visibility
	uint8_t nVisibility = atoi(data->getNextToken());

	//Set data in object
	if(nBlindNo == 1)
	{
		m_pServo1->setDirection(static_cast<CServo::Direction>(nDirection));
		m_pServo1->setOpenPercent(nOpenPercent);
		m_pServo1->setSpeedPercent(nSpeedPercent);
		m_pServo1->setAutoDrive(static_cast<bool>(nAutoDrive));
		m_pServo1->setVisibility(static_cast<CServo::Visibility>(nVisibility));
	}
	else if(nBlindNo == 2)
	{
		m_pServo2->setDirection(static_cast<CServo::Direction>(nDirection));
		m_pServo2->setOpenPercent(nOpenPercent);
		m_pServo2->setSpeedPercent(nSpeedPercent);
		m_pServo2->setAutoDrive(static_cast<bool>(nAutoDrive));
		m_pServo2->setVisibility(static_cast<CServo::Visibility>(nVisibility));
	}

	return true;
}

bool CUartParser::processCalibrate(CDataParser *data)
{
	//TODO - implement calibration
	return false;
}

bool CUartParser::processSetBlindType(CDataParser *data)
{
	char *cBlindNo = data->getNextToken();
	char *cBlindType = data->getNextToken();


//	CUart::getInstance()->puts(cBlindNo);//(dataParser.getNextToken());
//	CUart::getInstance()->puts("\r\n");
//
//	CUart::getInstance()->puts(cBlindType);//(dataParser.getNextToken());
//	CUart::getInstance()->puts("\r\n");

	uint8_t nBlindNo = atoi(cBlindNo);
	uint8_t nBlindType = atoi(cBlindType);

	CServo::BlindType blindType = static_cast<CServo::BlindType>(nBlindType);

	if(nBlindNo == 1)
	{
		PORTB ^= (1 << PB7);
		m_pServo1->setBlindType(blindType);
	}

	else if(nBlindNo == 2)
		m_pServo2->setBlindType(blindType);

	return true;
}

void CUartParser::event()
{
	if(isReadyForProcessData())
	{

		CUart::getInstance()->putint(test++, 10);
		CUart::getInstance()->puts("\r\n");

		CUart::getInstance()->puts(m_buffer);//(dataParser.getNextToken());
		CUart::getInstance()->puts("\r\n");

		//Clear error flag
		setError(false);

		//Process it
		CInternalConnParser internalConnParser;
		ParseResult result = internalConnParser.parseInputMessage(m_buffer);

		if(result != ParseResult::Ok)
		{
			setError(true);
			processStepTransition(2);
			return;
		}

		CDataParser dataParser;
		dataParser.checkTrimmingChars(internalConnParser.getBuffer(), '#');
		dataParser.parseData(internalConnParser.getBuffer(), "#");

		//Check if message is request
		if(strcmp("req", dataParser.getNextToken()) != 0)
		{
			setError(true);
			processStepTransition(2);
			return;
		}

		char dataBuffer[30] = {0};
		strcpy(dataBuffer, dataParser.getNextToken());

		result = dataParser.checkTrimmingChars(dataBuffer, '(', ')' );
		char *trimmedBuff = nullptr;
		if(result == ParseResult::Ok)
		{
			//Delete first and last chars
			trimmedBuff = dataBuffer;
			trimmedBuff++[strlen(dataBuffer)-1] = 0;
		}

		//Parse data
		dataParser.parseData(trimmedBuff, "!");

		char *cMsgType = dataParser.getNextToken();


		CUart::getInstance()->puts(cMsgType);
		CUart::getInstance()->puts("\r\n");

		//Decide how process message
		clearResponseBuffer();
		if(strcmp("sayHello", cMsgType) == 0)
		{
			//Process sayHello
			if(!processSayHello(&dataParser))
				setError(true);

		}
		else if(strcmp("getBlindType", cMsgType) == 0)
		{

			if(!processGetBlindType(&dataParser))
				setError(true);
		}
		else if(strcmp("getBlindState", cMsgType) == 0)
		{
			if(!processGetBlindState(&dataParser))
				setError(true);
		}
		else if(strcmp("setBlindState", cMsgType) == 0)
		{
			if(!processSetblindState(&dataParser))
				setError(true);
		}
		else if(strcmp("calibrate", cMsgType) == 0)
		{
			if(!processCalibrate(&dataParser))
				setError(true);
		}
		else if(strcmp("setBlindType", cMsgType) == 0)
		{

			if(!processSetBlindType(&dataParser))
				setError(true);
		}
		else
		{
			setError(true);
		}

		//dataParser.getNextToken()

		//CUart::getInstance()->puts(dataParser.getNextToken());
		//CUart::getInstance()->puts("\r\n");




		//Reset flag
		processStepTransition(2);
	}



	if(isReadyForProcessResponse())
	{

		sendCallbackMsg();
		//Reset flag
	}

	//clear all steps
	clearStepTransition();
}


