/*
 * dataParser.h
 *
 *  Created on: Jun 14, 2018
 *      Author: damian
 */

#ifndef SRC_PARSER_DATAPARSER_H_
#define SRC_PARSER_DATAPARSER_H_

#include <avr/io.h>

enum class TokenParseResult
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

class CTokenParser {
public:
	//Cnstructors
	CTokenParser();	//Prevent consttuction but allow in getInstance
	~CTokenParser();					//Prevent unwanted destruction

	TokenParseResult checkTrimmingChars(const char *data, char delim1 = ' ', char delim2 = '\0');
	TokenParseResult parseData(char *data, const char *delim = " ");
	char *getNextToken();
	void clean();

	//Operations
private:
	void resetPos();
	void cleanBuffer();

	//members
private:
	static const uint8_t m_sBufferRows = 6;
	static const uint8_t m_sBufferColumns = 18;
	char m_buffer[m_sBufferRows][m_sBufferColumns] ;
	uint8_t m_nPos = 0;
};

#endif /* SRC_PARSER_DATAPARSER_H_ */
