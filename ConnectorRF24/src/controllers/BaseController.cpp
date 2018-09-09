/*
 * BaseController.cpp
 *
 *  Created on: Sep 2, 2018
 *      Author: damian
 */

#include "BaseController.h"

CBaseController::CBaseController() {
	m_radioParser.registerTokenParser(&m_tokenParser);
	m_uartParser.registerTokenParser(&m_tokenParser);
}

CBaseController::~CBaseController() {
	// TODO Auto-generated destructor stub
}

//Notofications
void CBaseController::radioDataReady(char *pData)
{
	clearNotofications();
	m_bRadioDataReady = true;
	m_pData = pData;
}

void CBaseController::uartDataReady(char *pData)
{
	clearNotofications();
	m_bUartDataReady = true;
	m_pData = pData;
}

char *CBaseController::getData()
{
	return m_pData;
}

void CBaseController::clearNotofications()
{
	m_bRadioDataReady = false;
	m_bUartDataReady = false;
}
