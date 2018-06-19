/*
 * dataParser.h
 *
 *  Created on: Jun 14, 2018
 *      Author: damian
 */

#ifndef SRC_PARSER_DATAPARSER_H_
#define SRC_PARSER_DATAPARSER_H_

#include <avr/io.h>
#include "resultTypes.h"

class CDataParser {
public:
	//Cnstructors
	CDataParser();	//Prevent consttuction but allow in getInstance
	~CDataParser();					//Prevent unwanted destruction

	DataParseResult checkTrimmingChars(char *data, char delim1 = ' ', char delim2 = '\0');
	DataParseResult parseData(char *data, const char *delim = " ");
	char *getNextToken();
	void clean();

	//Operations
private:
	void resetPos();
	void cleanBuffer();

	//members
private:
	static const uint8_t m_sBufferRows = 20;
	static const uint8_t m_sBufferColumns = 18;
	char m_buffer[m_sBufferRows][m_sBufferColumns] ;
	uint8_t m_nPos = 0;
};

#endif /* SRC_PARSER_DATAPARSER_H_ */
