/*
 * ModelSataContainer.h
 *
 *  Created on: Sep 22, 2018
 *      Author: damian
 */

#pragma once

#include "../servo/servo.h"

struct SBlindType
{
	uint8_t nBlindNo = 0;
	ServoEnum::BlindType blindType;
};

struct SBlindState
{
	uint8_t nBlindNo = 0;
	ServoEnum::Direction direction = ServoEnum::Direction::Stop;
	uint8_t nOpenPercent = 0;
	ServoEnum::Visibility visibility = ServoEnum::Visibility::None;
	bool bAutoDrive = true;
	bool bIsWindowClosed = false;
};

struct SBlindCalibrate
{
	uint8_t nBlindNo = 0;
	uint8_t nCalibrateStep = 0;
};
