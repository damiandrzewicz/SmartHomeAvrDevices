/*
 * resultTypes.h
 *
 *  Created on: Jun 18, 2018
 *      Author: damian
 */

#ifndef SRC_DATAPARSER_RESULTTYPES_H_
#define SRC_DATAPARSER_RESULTTYPES_H_

enum class DataParseResult
{
	NullDataPointer,		//0
	EmptyDataPointer,		//1
	BadFirstChar,			//2
	BadLastChar,			//3
	EmptyRequest,			//4
	EmptyValue,				//5
	BadRequest,				//6
	Ok
};

enum class UartParserResult
{
	Dummy = DataParseResult::Ok,	//7
	BadMessageType,					//8
	BadOperationType,				//9
	EmptyData,						//10
	ParsingError,					//11
	Ok
};

enum class ControllerResult
{
	Dummy = UartParserResult::Ok,	//12
	Timeout,						//13
	BadMessageType,					//14
	BadOperationType,				//15
	Ok								//16
};



#endif /* SRC_DATAPARSER_RESULTTYPES_H_ */
