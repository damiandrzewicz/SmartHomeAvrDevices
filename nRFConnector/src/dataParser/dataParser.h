/*
 * dataParser.h
 *
 *  Created on: Jun 14, 2018
 *      Author: damian
 */

#ifndef SRC_PARSER_DATAPARSER_H_
#define SRC_PARSER_DATAPARSER_H_

#include <avr/io.h>


enum class ParseResult
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

class CDataParser {
public:


	ParseResult checkTrimmingChars(char *data, char delim1 = ' ', char delim2 = '\0');
	ParseResult parseData(char *data, const char *delim = " ");
	char *getNextToken();
	void cleanBuffer();

public:
	//Cnstructors
	CDataParser();	//Prevent consttuction but allow in getInstance
	CDataParser(const CDataParser&);	//Prevent construction by copying
	CDataParser& operator=(const CDataParser&);	//Prevent assigment
	~CDataParser();					//Prevent unwanted destruction

private:
	void resetPos();

private:
	static const uint8_t m_sBufferRows = 20;
	char m_buffer[m_sBufferRows][18] ;
	uint8_t m_nPos = 0;
};

#endif /* SRC_PARSER_DATAPARSER_H_ */
