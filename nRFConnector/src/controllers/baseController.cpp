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

template <typename T>
CBaseController<T>::CBaseController() {
	m_nErrorNo = static_cast<int8_t>(ErrorType::Ok);

}

template <typename T>
CBaseController<T>::~CBaseController() {
	// TODO Auto-generated destructor stub
}

//template <typename T>
//T *CBaseController<T>::getInstance();
//{
//	static T instance;
//	return instance;
//}

template <typename T>
void CBaseController<T>::setOperationName(char *op)
{
	m_sOperationName = op;
}

template <typename T>
char *CBaseController<T>::getOperationName()
{
	return m_sOperationName;
}

//Uart
template <typename T>
void CBaseController<T>::setReadyForProcessUart(bool value)
{
	m_bReadyForProcessUart = value;
}

template <typename T>
bool CBaseController<T>::isReadyForProcessUart()
{
	return m_bReadyForProcessUart;
}

template <typename T>
void CBaseController<T>::setDataInUartBuffer(char *data)
{
	m_uartBuffer = data;
}

template <typename T>
char *CBaseController<T>::getUartdataBuffer()
{
	return m_uartBuffer;
}

template <typename T>
void CBaseController<T>::resetError()
{
	setError(ErrorType::Ok);
}

template <typename T>
void CBaseController<T>::setError(ErrorType err)
{
	setError(static_cast<int8_t>(err));
}

template <typename T>
void CBaseController<T>::setError(int8_t err)
{
	m_nErrorNo = static_cast<int8_t>(err);
	char valBuff[5];
	itoa(static_cast<int8_t>(m_nErrorNo), valBuff, 10);
	strcpy(getBufferPtr(), valBuff);
}

template <typename T>
bool CBaseController<T>::isError()
{
	int8_t nOk = static_cast<int8_t>(ErrorType::Ok);
	return (m_nErrorNo != nOk);
}

template <typename T>
void CBaseController<T>::setRadioDataReceived(bool val)
{
	m_bRadioDataReceived = val;
}

template <typename T>
bool CBaseController<T>::isRadioDataReceived()
{
	return m_bRadioDataReceived;
}

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
char *CBaseController<T>::getBufferPtr()
{
	return m_dataBuffer;
}

template <typename T>
void CBaseController<T>::setMessageInBuffer(char *msg)
{
	strcpy(getBufferPtr(), msg);
}

template <typename T>
bool CBaseController<T>::isTimeout()
{
	if(m_nTimerValue > (m_sTimeout / 10))
		return true;
	else
		return false;
}
//template <typename T>
//void CBaseController<T>::processSendData()
//{
//	CNrf::getInstance()->sendDataToAir(getBufferPtr());
//	startTimer();
//}

template <typename T>
void CBaseController<T>::timerCallback()
{
	incrementtimerTick();
}

template <typename T>
void CBaseController<T>::nrfCallback(void * nRF_RX_buff , uint8_t len )
{
	setMessageInBuffer(static_cast<char*>(nRF_RX_buff));
	setRadioDataReceived(true);
}

#endif
