/*
 * dataParser.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: damian
 */

#include "dataParser.h"
//#include "../uart/uart.h"

#include <string.h>
#include <stdlib.h>

CDataParser::CDataParser() {
	// TODO Auto-generated constructor stub

}

CDataParser::~CDataParser() {
	// TODO Auto-generated destructor stub
}

DataParseResult CDataParser::checkTrimmingChars(char *data, char delim1, char delim2)
{
	//Get size
	uint8_t len = strlen(data);

	if(delim2 == '\0')
		delim2 = delim1;

	//Check data object
	if(data == nullptr)
		return DataParseResult::NullDataPointer;
	else if(len == 0)
		return DataParseResult::EmptyDataPointer;

	//Check first character
	if(data[0] != delim1)
		return DataParseResult::BadFirstChar;

	//Check last character
	if(data[len-1] != delim2)
		return DataParseResult::BadLastChar;

	return DataParseResult::Ok;
}

DataParseResult CDataParser::parseData(char *data, const char *delim)
{
	//Reset pos
	resetPos();
	cleanBuffer();

	uint8_t nPos = 0;
	char *token = strtok(data, delim);

	while(token != NULL)
	{
		strcpy(m_buffer[nPos++], token);
		//m_buffer[nPos++] = token;
		token = strtok(NULL, delim);
	}

	//CUart::getInstance()->puts(m_buffer[1]);
	//CUart::getInstance()->puts("\r\n");

	//PORTB ^= (1 << PB7);
	return DataParseResult::Ok;
}

char *CDataParser::getNextToken()
{
	if(m_nPos < m_sBufferRows)
		return m_buffer[m_nPos++];
	else
		return nullptr;
}

void CDataParser::cleanBuffer()
{
	memset(m_buffer, 0, sizeof m_buffer);
}


void CDataParser::resetPos()
{
	m_nPos = 0;
}

void CDataParser::clean()
{
	cleanBuffer();
	resetPos();
}

