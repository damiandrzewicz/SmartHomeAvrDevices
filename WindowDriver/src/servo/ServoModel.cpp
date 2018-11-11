/*
 * ServoModel.cpp
 *
 *  Created on: Oct 21, 2018
 *      Author: damian
 */

#include "ServoModel.h"

#include "../uart/uart.h"

TBlindMetadata EEMEM eem_blindMetadataArr[2];
TBlindCalibrateMetadata EEMEM eem_blindCalibrateMetadata[2];

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

//void CBlindMetadata::setBlindType(WindowData::BlindType blindType){ m_metadata.blindType = blindType; }
//WindowData::BlindType CBlindMetadata::getBlindType()const { return m_metadata.blindType; }
//
//
//void CBlindMetadata::setBlindVisibility(WindowData::Visibility val){ m_metadata.visibility = val; }
//WindowData::Visibility CBlindMetadata::getBlindVisibility()const { return m_metadata.visibility; }
//
//void CBlindMetadata::setInitialized(bool val){ m_metadata.isMetadataInitialized = val; }
//bool CBlindMetadata::isInitialized() const{ return m_metadata.isMetadataInitialized; }

//void CBlindMetadata::setBlindType(WindowData::BlindType blindType)
//{
////	CUart::getInstance()->puts("Setter blindType: ");
////	CUart::getInstance()->putint(static_cast<uint8_t>(blindType), 10);
////	CUart::getInstance()->puts("\r\n");
//	m_blindType = blindType;
//}
//WindowData::BlindType CBlindMetadata::getBlindType()const { return m_blindType; }
//
//
//void CBlindMetadata::setBlindVisibility(WindowData::Visibility val){ m_blindVisibility = val; }
//WindowData::Visibility CBlindMetadata::getBlindVisibility()const { return m_blindVisibility; }
//
//void CBlindMetadata::setInitialized(bool val){ m_nIsInitialized = val; }
//bool CBlindMetadata::isInitialized() const{ return m_nIsInitialized; }

//void CBlindMetadata::printData()
//{
//	CUart::getInstance()->puts("***printData***\r\n");
//	CUart::getInstance()->puts("blindType: ");
//	CUart::getInstance()->putint(static_cast<uint8_t>(getBlindType()), 10);
//	CUart::getInstance()->puts("\r\n");
//
//	CUart::getInstance()->puts("visibility: ");
//	CUart::getInstance()->putint(static_cast<uint8_t>(getBlindVisibility()), 10);
//	CUart::getInstance()->puts("\r\n");
//
//	CUart::getInstance()->puts("isMetadataInitialized: ");
//	CUart::getInstance()->putint(static_cast<uint8_t>(isInitialized()), 10);
//	CUart::getInstance()->puts("\r\n");
//}

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
	readBlindMetadataFromEeprom();
}

void CServoModel::writeBlindMetadataToEeprom()
{
	//CUart::getInstance()->puts("writeBlindMetadataToEeprom\r\n");
	TBlindMetadata temp = getBlindMetadataObject();
	eeprom_write_block(&temp, &eem_blindMetadataArr[getBlindNumber() - 1], sizeof(TBlindMetadata));
}

TBlindMetadata CServoModel::readBlindMetadataFromEeprom()
{
	TBlindMetadata temp;
	eeprom_read_block(&temp, &eem_blindMetadataArr[getBlindNumber() - 1], sizeof(TBlindMetadata));

//	CUart::getInstance()->puts("readBlindMetadataFromEeprom\r\n");
//
//	CUart::getInstance()->puts("blindNo:");
//	CUart::getInstance()->putint(getBlindNumber(), 10);
//	CUart::getInstance()->puts("r\n");
//
//	CUart::getInstance()->puts("isMetadataInitialized:");
//	CUart::getInstance()->putint(temp.isMetadataInitialized, 10);
//	CUart::getInstance()->puts("r\n");

	if(temp.isMetadataInitialized)
	{
		setBlindMetadataObject(temp);
	}

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
	//setCalibrateMetadataObject(temp);
	return temp;
}


