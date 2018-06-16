/*
 * slaveController.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

#include "slaveController.h"
#include "../nrf/nrf.h"


CSlaveController::CSlaveController() {
	// TODO Auto-generated constructor stub

}

CSlaveController::~CSlaveController() {
	// TODO Auto-generated destructor stub
}

void CSlaveController::processSendData()
{
	CNrf::getInstance()->sendDataToAir(getBufferPtr());
	//setWaitingForRadioResponse(true);
	//startTimer();
}


void CSlaveController::controllerEvent()
{

}

//Callbacks
void CSlaveController::uartCallback(char *data)
{

}
