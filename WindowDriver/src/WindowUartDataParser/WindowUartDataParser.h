/*
 * ModelDataParser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: damian
 */

#pragma once

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "ParserInterface/CParserInterface.h"
#include "../servo/servo.h"
#include "../servo/ServoModel.h"

class CWindowUartDataParser : public CParserInterface {
public:
	enum class OperationName
	{
        SetBlindMetadata,       //Set Type and Visibility   ---> return OK or ERR
        GetBlindMetadata,       //Get Type and Visibility   ---> return Type and Visibility or ERR
        ManualDrive,            //Drive servo manual        ---> return OK or ERR   (set open/close/stop)
        SetBlindState,          //Set openPercent and openSpeed ---> return OK or ERR
        GetBlindState,          //Get openPercent and openSpeed or ERR
        SetCalibrateStep,       //Set calibrate step        ---> return Ok or ERR
        NotSupported
	};

	enum class WindowError
	{
		BlindBlocked = Error::END_OF_ERROR
	};

	CWindowUartDataParser();
	~CWindowUartDataParser();

	virtual bool parse(char *pData) override;
	bool createMessage(OperationName opName, OperationDirection opDir, char *pContext, char *pResult = nullptr);

	virtual bool createErrorMsg(uint8_t errNo, char *pResult) override;

	OperationName getOperationName();
	char *getOperationNameText(OperationName op);

	bool parseBlindNo(uint8_t &nBlindNo);

	//Incoming
	bool parseSetBlindMetadata(CBlindMetadata &refBlindMetadata);
	bool parseGetBlindMetadata(CBlindMetadata &refBlindMetadata);

	bool parseManualDrive(CBlindManualDrive &refManualDrive);

	bool parseGetBlindState(CBlindState &refBlindState);
	bool parseSetBlindState(CBlindState &refBlindState);

	bool parseSetCalibrate(CBlindCalibrate &refBlindCalibrate);

	//bool parseSetBlindType(SBlindType &refBlindState);
	//bool parseSetBlindState()

	//Outcoming
	bool createGetBlindMetadataContext(CBlindMetadata &refBlindMetadata, char *pResult = nullptr);
	bool createGetBlindStateContext(CBlindState &refBlindState, char *pResult = nullptr);

protected:

	void setOperationName(OperationName op);
	OperationName parseOperationName(char *pOperationName);

	char m_modelContextArr[15];
};

