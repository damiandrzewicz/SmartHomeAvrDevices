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

void CBlindMetadata::setBlindMetadataObject(const TBlindMetadata &val){ m_metadata = val; }
TBlindMetadata &CBlindMetadata::getBlindMetadataObject(){ return m_metadata; }

//void CBlindMetadata::setBlindType(WindowData::BlindType blindType){ m_blindType = blindType; }
//WindowData::BlindType CBlindMetadata::getBlindType()const { return m_blindType; }
//void CBlindMetadata::setInitialized(bool val){ m_bIsInitialized = val; }
//
//void CBlindMetadata::setBlindVisibility(WindowData::Visibility val){ m_blindVisibility = val; }
//WindowData::Visibility CBlindMetadata::getBlindVisibility()const { return m_blindVisibility; }
//bool CBlindMetadata::isInitialized() const{ return m_bIsInitialized; }

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
void CBlindCalibrate::setCalibrateMetadataObject(const TBlindCalibrateMetadata &val){ m_calibrateMetadata = val; }

uint8_t CBlindCalibrate::getCalibrateStep()const { return m_nCalibrateStep; }
TBlindCalibrateMetadata &CBlindCalibrate::getCalibrateMetadataObject(){ return m_calibrateMetadata; }

/*********************/
//CServoModel Class
/*********************/

CServoModel::CServoModel(uint8_t nBlindNo)
	: CBlindBase(nBlindNo),
	  CBlindMetadata(nBlindNo),
	  CBlindState(nBlindNo),
	  CBlindCalibrate(nBlindNo),
	  CBlindManualDrive(nBlindNo)
{
}

void CServoModel::writeBlindMetadataToEeprom()
{
	eeprom_write_block((const void*)&getBlindMetadataObject(), (void*)&eem_blindMetadataArr[getBlindNumber() - 1], sizeof(TBlindMetadata));
}

TBlindMetadata CServoModel::readBlindMetadataFromEeprom()
{
	TBlindMetadata temp;
	eeprom_read_block((void*)&temp, (const void*)&eem_blindMetadataArr[getBlindNumber() - 1], sizeof(TBlindMetadata));
	setBlindMetadataObject(temp);
	return temp;
}

void CServoModel::writeBlindCalibrateMetadataToEeprom()
{
	eeprom_write_block((const void*)&getCalibrateMetadataObject(), (void*)&eem_blindCalibrateMetadata[getBlindNumber() - 1], sizeof(TBlindMetadata));
}

TBlindCalibrateMetadata CServoModel::readBlindCalibrateMetadataFromEeprom()
{
	TBlindCalibrateMetadata temp;
	eeprom_read_block((void*)&temp, (const void*)&eem_blindCalibrateMetadata[getBlindNumber() - 1], sizeof(TBlindMetadata));
	setCalibrateMetadataObject(temp);
	return temp;
}


