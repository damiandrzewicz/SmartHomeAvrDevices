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
	enum class MotorSide{ Left, Right, None };
};

template <class T>
class CNotofiedValue
{
public:
	CNotofiedValue(){}
	CNotofiedValue(T v){m_value = v;}

	void setHasChanged(bool value){ m_bHasChanged = value; }
	bool hasChanged(){ return m_bHasChanged; }

	CNotofiedValue<T> operator=(const T v)
	{
		m_value = v;
		return *this;
	}



	void setValue(T val){ m_value = val; setHasChanged(true); }
	T getValue(){ return m_value; }


private:
	bool m_bHasChanged = false;
	T m_value;
};




class CBlindBase
{
public:
	CBlindBase(uint8_t nBlindNo);
	virtual ~CBlindBase();

	void setBlindNumber(uint8_t val);
	uint8_t getBlindNumber()const ;
	uint8_t &getBlindNumber();

private:
	uint8_t m_nBlindNo = 0;
};

struct TBlindMetadata
{
	//bool isMetadataInitialized = false;
	WindowData::BlindType blindType = WindowData::BlindType::DayNight;
	WindowData::MotorSide motorSide = WindowData::MotorSide::Left;
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

    WindowData::Visibility visibility = WindowData::Visibility::Night;

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
    CNotofiedValue<WindowData::Direction> getManualDriveDirection()const;
    CNotofiedValue<WindowData::Direction> &getManualDriveDirection();

private:
    CNotofiedValue<WindowData::Direction> m_manualDirection = WindowData::Direction::Stop;
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
   CNotofiedValue<uint8_t> &getOpenPercent();
   CNotofiedValue<uint8_t> getOpenPercent()const;
   uint8_t getOpenSpeed()const;
   bool isWindowClosed()const;

private:
   CNotofiedValue<uint8_t> m_nOpenPercent = 0;
   uint8_t m_nOpenSpeed = 0;
   bool m_bIsWindowClosed = false;
};

static const uint8_t POSITION_ARRAY_SIZE = 40;

struct TBlindCalibrateMetadata
{
	bool bIsCalibrated = /*false*/ true;
	//int32_t maxEncoderCounter = 77596;
	//uint16_t singleEncoderStep = 1615;
	int16_t offsetEncoder = -600;

	uint16_t positionArray[POSITION_ARRAY_SIZE] = {
			880,
			1751,
			2631,
			3548,
			4444,
			5405,
			6357,
			7335,
			8331,
			9362,
			10433,
			11526,
			12638,
			13796,
			14995,
			16225,
			17492,
			18835
	};
	uint8_t nRegisteredSteps = 18;

};

class CBlindCalibrate : virtual public CBlindBase
{
public:

	enum class CalibrationStep
	{
		Idle = 100,
		ActivateCalibration = 101,
		StartPoint = 0,
		EndPoint = POSITION_ARRAY_SIZE - 1,
		PointsReady = 103,
		FullClose = 104,
		Offset = 105,
		StartPosition = 106,
		Finished = 107
	};

   CBlindCalibrate(uint8_t nBlindNo = 0);
   virtual ~CBlindCalibrate();

   virtual void setCalibrateStep(uint8_t val);
   void setCalibrateMetadataObject(const TBlindCalibrateMetadata &val);

   uint8_t getCalibrateStep()const;
   TBlindCalibrateMetadata &getCalibrateMetadataObject();

   void setPositionArrayValue(uint8_t nPos, uint32_t nValue);
   uint32_t getPositionArrayValue(uint8_t nPos);

private:
   uint8_t m_nCalibrateStep = static_cast<uint8_t>(CalibrationStep::Idle);
   TBlindCalibrateMetadata m_calibrateMetadata;

};

class CServoModel : public CBlindMetadata, public CBlindState, public CBlindCalibrate, public CBlindManualDrive
{
public:
	CServoModel(uint8_t nBlindNo = 0);

	void writeBlindMetadataToEeprom();
	void readBlindMetadataFromEeprom();

	void writeBlindCalibrateMetadataToEeprom();
	void readBlindCalibrateMetadataFromEeprom();

};

