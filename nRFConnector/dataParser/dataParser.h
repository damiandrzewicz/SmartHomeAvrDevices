/*
 * CDataParser.h
 *
 *  Created on: Apr 7, 2018
 *      Author: damian
 */

#ifndef CDATAPARSER_H_
#define CDATAPARSER_H_

#include <avr/io.h>

class CDataParser {
public:
	enum class ParseResult
	{
		NullDataPointer,
		EmptyDataPointer,
		BadFirstChar,
		BadLastChar,
		EmptyRequest,
		EmptyOperation,
		EmptyValue,
		Ok
	};

	ParseResult parseData(char *data);
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
	static const uint8_t m_sBufferRows = 3;
	char *m_buffer[m_sBufferRows] = {};
	uint8_t m_nPos = 0;
};

#endif /* CDATAPARSER_H_ */
