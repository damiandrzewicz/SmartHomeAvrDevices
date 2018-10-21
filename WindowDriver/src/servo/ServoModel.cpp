/*
 * ServoModel.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: damian
 */

#include "ServoModel.h"

/*********************/
//CBlindTypeBase Class
/*********************/
CBlindBase::CBlindBase(uint8_t nBlindNo) { m_nBlindNo = nBlindNo; }
CBlindBase::~CBlindBase(){}

uint8_t &CBlindBase::getBlindNumber(){ return m_nBlindNo; }
uint8_t CBlindBase::getBlindNumber()const { return m_nBlindNo; }

/*********************/
//CBlindMetadata Class
/*********************/
CBlindMetadata::CBlindMetadata(uint8_t nBlindNo) : CBlindBase(nBlindNo){}
CBlindMetadata::~CBlindMetadata(){}

void CBlindMetadata::setBlindType(WindowData::BlindType blindType){ m_blindType = blindType; }
WindowData::BlindType CBlindMetadata::getBlindType()const { return m_blindType; }

void CBlindMetadata::setBlindVisibility(WindowData::Visibility val){ m_blindVisibility = val; }
WindowData::Visibility CBlindMetadata::getBlindVisibility()const { return m_blindVisibility; }

/*********************/
//CBlindManualDrive Class
/*********************/
CBlindManualDrive::CBlindManualDrive(uint8_t nBlindNo) : CBlindBase(nBlindNo){}
CBlindManualDrive::~CBlindManualDrive(){}

void CBlindManualDrive::setManualDriveDirection(WindowData::Direction dir){ m_manualDirection = dir; }
WindowData::Direction CBlindManualDrive::getManualDriveDirection()const{ return m_manualDirection; }

/*********************/
//CBlindState Class
/*********************/
CBlindState::CBlindState(uint8_t nBlindNo): CBlindBase(nBlindNo){}
CBlindState::~CBlindState(){}


//Setters
bool CBlindState::setOpenPercent(uint8_t val){
	if(val > 100) return false;
	m_nOpenPercent = val;    return true; }
bool CBlindState::setOpenSpeed(uint8_t val){
	if(val > 100) return false;
	m_nOpenSpeed = val; return true; }

void CBlindState::setIsWindowClosed(bool val){ m_bIsWindowClosed = val; }

//Getters
uint8_t CBlindState::getOpenPercent()const { return m_nOpenPercent; }
uint8_t CBlindState::getOpenSpeed()const { return m_nOpenSpeed; }
bool CBlindState::isWindowClosed()const { return m_bIsWindowClosed; }

/*********************/
//CBlindCalibrate Class
/*********************/
CBlindCalibrate::CBlindCalibrate(uint8_t nBlindNo) : CBlindBase(nBlindNo){}
CBlindCalibrate::~CBlindCalibrate(){}

void CBlindCalibrate::setCalibrateStep(uint8_t val){ m_nCalibrateStep = val; }
uint8_t CBlindCalibrate::getCalibrateStep()const { return m_nCalibrateStep; }

/*********************/
//CServoModel Class
/*********************/

CServoModel::CServoModel(uint8_t nBlindNo)
	: CBlindBase(nBlindNo),
	  CBlindMetadata(nBlindNo),
	  CBlindState(nBlindNo),
	  CBlindCalibrate(nBlindNo)
{
}


