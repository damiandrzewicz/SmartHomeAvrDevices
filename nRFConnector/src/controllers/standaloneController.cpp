/*
 * CStandaloneController.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

#include <string.h>
#include "standaloneController.h"
#include "../nrf/nrf.h"


CStandaloneController::CStandaloneController() {
	// TODO Auto-generated constructor stub

}

CStandaloneController::~CStandaloneController() {
	// TODO Auto-generated destructor stub
}

void CStandaloneController::processSendData()
{
	CNrf::getInstance()->sendDataToAir(getBufferPtr());
}

void CStandaloneController::setRequestInBufferReady(bool val)
{
	m_bRequestInBufferReady = val;
}

bool CStandaloneController::isRequestInBufferReady()
{
	return m_bRequestInBufferReady;
}

void CStandaloneController::setReadyForProcessResponse(bool val)
{
	m_bReadyForProcessResponse = val;
}

bool CStandaloneController::isReadyForProcessResponse()
{
	return m_bReadyForProcessResponse;
}

void CStandaloneController::prepareResponseMsgFromBuffer(char *buff)
{
	strcpy(buff, "@response");

	//Check if Error occured
	if(isError())
	{
		strcat(buff, "@err@");
	}
	else
	{
		strcat(buff, "@ok@");
	}

	//Add info data
	strcat(buff, getBufferPtr());


	if(strcmp(getOperationName(), "") != 0)
	{
		strcat(buff, "@");
		strcat(buff, getOperationName());
	}

	strcat(buff, "@");
}

void CStandaloneController::controllerEvent()
{
	if(isRadioDataReceived())
	{
		//process it
		setRadioDataReceived(false);
		char buf[20];
		strcpy(buf, "testxD from damian123");
		setMessageInBuffer(buf);
		//CNrf::getInstance()->sendDataToAir(getBufferPtr());
		setReadyForProcessResponse(true);
	}

	if(isReadyForProcessResponse())
	{
		//Prepare response
		char response[100];
		prepareResponseMsgFromBuffer(response);

		//Send data by nrf
		strcpy(getBufferPtr(), response);
		processSendData();

		//Reset ready for response state
		setReadyForProcessResponse(false);

		//Reset error if occured
		if(isError())
		{
			resetError();
		}
	}
}

//No implementation - UART not used
void CStandaloneController::uartCallback(char *data){}
