/*
 * baseController.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: damian
 */

//#include "baseController.h"

#ifdef CONTROLLERS_BASECONTROLLER_H_

#include <string.h>

#include "../nrf/nrf.h"
#include "../nrf/nRF24L01_memory_map.h"
#include "../uart/uart.h"
#include "../timer/timer.h"
#include "../dataParser/resultTypes.h"

template <typename T>
CBaseController<T>::CBaseController() {
	//m_nErrorNo = static_cast<int8_t>(ErrorType::Ok);

}

template <typename T>
CBaseController<T>::~CBaseController() {
	// TODO Auto-generated destructor stub
}

/********************************************
 * ****** PROCESSING MESSAGES ***************
 ********************************************/
//template <typename T>
//void CBaseController<T>::setBusy(bool value)
//{
//	m_bBusy = value;
//}
//
//template <typename T>
//bool CBaseController<T>::isBusy()
//{
//	return m_bBusy;
//}

template <typename T>
void CBaseController<T>::resetError()
{
	m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);
}

template <typename T>
void CBaseController<T>::setError(int8_t err){
	m_nErrorNo = err;
}

//template <typename T>
//ControllerResult CBaseController<T>::getError()
//{
//	return m_controllerState;
//}

template <typename T>
int8_t CBaseController<T>::getErrorNo()
{
	return m_nErrorNo;
}

template <typename T>
bool CBaseController<T>::isError()
{
	return (m_nErrorNo != static_cast<bool>(ControllerResult::Ok)) ? true : false;
}

/********************************************
 * **************** TIMER *******************
 ********************************************/
template <typename T>
uint8_t CBaseController<T>::getTimerHandle() const
{
	return m_sTimerHandle;
}

template <typename T>
void CBaseController<T>::startTimer()
{
	resetTimerValue();
	CTimer2::getInstance()->Enable(getTimerHandle());
}

template <typename T>
void CBaseController<T>::stopTimer()
{
	CTimer2::getInstance()->Disable(getTimerHandle());
}

template <typename T>
void CBaseController<T>::resetTimerValue()
{
	m_nTimerValue = 0;
}

template <typename T>
void CBaseController<T>::incrementtimerTick()
{
	m_nTimerValue++;
}

template <typename T>
bool CBaseController<T>::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}

/********************************************
 * ************* MAIN BUFFER ****************
 ********************************************/
template <typename T>
char *CBaseController<T>::getBufferPtr()
{
	return m_dataBuffer;
}

template <typename T>
void CBaseController<T>::setDataInBuffer(char *msg)
{
	strcpy(getBufferPtr(), msg);
}

/********************************************
 * ************** OPERATIONS ****************
 ********************************************/
template <typename T>
void CBaseController<T>::processSendDataToAir()
{
	CNrf::getInstance()->sendDataToAir(getBufferPtr());
	startTimer();
}

template <typename T>
void CBaseController<T>::processSetTransmitterAddress()
{
	CNrf::getInstance()->setTransmitterAdres(getBufferPtr());
	CNrf::getInstance()->setReciverAddres(RX_ADDR_P0, getBufferPtr());
}

template <typename T>
void CBaseController<T>::clean()
{
	m_currentOperationType = CInternalUartParser::OperationType::Unspecified;
	m_nErrorNo =  static_cast<int8_t>(ControllerResult::Ok);
}

/********************************************
 * ************** CALLBACKS *****************
 ********************************************/
template <typename T>
void CBaseController<T>::uartCallback(char *data)
{
	setDataInBuffer(data);
	setReadyForProcessUartReceivedData(true);
}

template <typename T>
void CBaseController<T>::timerCallback()
{
	incrementtimerTick();
}

template <typename T>
void CBaseController<T>::nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	setDataInBuffer(static_cast<char*>(nRF_RX_buff));
	setReadyForProcessReceivedRadioData(true);
}



#endif
