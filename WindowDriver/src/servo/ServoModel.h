/*
 * ServoModel.h
 *
 *  Created on: Oct 21, 2018
 *      Author: damian
 */

#pragma once

#include <avr/eeprom.h>
#include <avr/io.h>
//#include "servo.h"



class WindowData
{
public:
	enum class Direction{ Open, Close, Stop};
	enum class Visibility{ Day, Night, None };
	enum class BlindType{ DayNight, FullBlackout, None };
};

class CBlindBase
{
public:
	CBlindBase(uint8_t nBlindNo);
	virtual ~CBlindBase();

	//void setBlindNumber(uint8_t val);
	uint8_t getBlindNumber()const ;
	uint8_t &getBlindNumber();

private:
	uint8_t m_nBlindNo = 0;
};

struct TBlindMetadata
{
	bool isMetadataInitialized = false;
	WindowData::BlindType blindType = WindowData::BlindType::None;
	WindowData::Visibility visibility = WindowData::Visibility::None;
};

class CBlindMetadata : virtual public CBlindBase
{
public:
	CBlindMetadata(uint8_t nBlindNo = 0);
    virtual ~CBlindMetadata();

    void setBlindMetadataObject(const TBlindMetadata &val);
//    virtual void setBlindType(WindowData::BlindType blindNo);
//    virtual void setBlindVisibility(WindowData::Visibility val);
//    void setInitialized(bool val);

    TBlindMetadata &getBlindMetadataObject();
//    WindowData::BlindType getBlindType()const;
//    WindowData::Visibility getBlindVisibility()const;
//    bool isInitialized() const;

    //void printData();


private:
	//WindowData::BlindType m_blindType = WindowData::BlindType::None;
	//WindowData::Visibility m_blindVisibility = WindowData::Visibility::None;
	//bool m_nIsInitialized = false;
    TBlindMetadata m_metadata;
};

class CBlindManualDrive : virtual public CBlindBase
{
public:
    CBlindManualDrive(uint8_t nBlindNo = 0);
    virtual ~CBlindManualDrive();

    virtual void setManualDriveDirection(WindowData::Direction dir);
    WindowData::Direction getManualDriveDirection()const;

private:
	WindowData::Direction m_manualDirection = WindowData::Direction::Stop;
};

class CBlindState : virtual public CBlindBase
{
public:
   CBlindState(uint8_t nBlindNo = 0);
   virtual ~CBlindState();

   //Setters
   virtual bool setOpenPercent(uint8_t val);
   virtual bool setOpenSpeed(uint8_t val);
   virtual void setIsWindowClosed(bool val);

   //Getters
   uint8_t getOpenPercent()const;
   uint8_t getOpenSpeed()const;
   bool isWindowClosed()const;

private:
   uint8_t m_nOpenPercent = 0;
   uint8_t m_nOpenSpeed = 0;
   bool m_bIsWindowClosed = false;
};

struct TBlindCalibrateMetadata
{
	uint8_t m_nIsCalibrated = false;
};

class CBlindCalibrate : virtual public CBlindBase
{
public:
   CBlindCalibrate(uint8_t nBlindNo = 0);
   virtual ~CBlindCalibrate();

   virtual void setCalibrateStep(uint8_t val);
   void setCalibrateMetadataObject(const TBlindCalibrateMetadata &val);

   uint8_t getCalibrateStep()const;
   TBlindCalibrateMetadata &getCalibrateMetadataObject();

private:
   uint8_t m_nCalibrateStep = 0;
   TBlindCalibrateMetadata m_calibrateMetadata;
};



class CServoModel : public CBlindMetadata, public CBlindState, public CBlindCalibrate, public CBlindManualDrive
{
public:
	CServoModel(uint8_t nBlindNo);

	void writeBlindMetadataToEeprom();
	TBlindMetadata readBlindMetadataFromEeprom();

	void writeBlindCalibrateMetadataToEeprom();
	TBlindCalibrateMetadata readBlindCalibrateMetadataFromEeprom();

};

