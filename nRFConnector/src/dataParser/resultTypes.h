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
	NullDataPointer,
	EmptyDataPointer,
	BadFirstChar,
	BadLastChar,
	EmptyRequest,
	EmptyValue,
	BadRequest,
	Ok
};

enum class UartParserResult
{
	Dummy = DataParseResult::Ok,
	EmptyMessageType,
	EmptyOperationType,
	EmptyData,
	ParsingError,
	Ok
};

enum class ControllerResult
{
	Dummy = UartParserResult::Ok,
	Timeout,
	BadMessageType,
	BadOperationType,
	Ok
};



#endif /* SRC_DATAPARSER_RESULTTYPES_H_ */
