/*
 * dataParser.cpp
 *
 *  Created on: Jun 14, 2018
 *      Author: damian
 */

#include "TokenParser.h"
//#include "../uart/uart.h"

#include <string.h>
#include <stdlib.h>

CTokenParser::CTokenParser() {
	// TODO Auto-generated constructor stub

}

CTokenParser::~CTokenParser() {
	// TODO Auto-generated destructor stub
}

TokenParseResult CTokenParser::checkTrimmingChars(const char *data, char delim1, char delim2)
{
	//Get size
	uint8_t len = strlen(data);

	if(delim2 == '\0')
		delim2 = delim1;

	//Check data object
	if(data == nullptr)
		return TokenParseResult::NullDataPointer;
	else if(len == 0)
		return TokenParseResult::EmptyDataPointer;

	//Check first character
	if(data[0] != delim1)
		return TokenParseResult::BadFirstChar;

	//Check last character
	if(data[len-1] != delim2)
		return TokenParseResult::BadLastChar;

	return TokenParseResult::Ok;
}

TokenParseResult CTokenParser::parseData(char *data, const char *delim)
{
	clean();

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
	return TokenParseResult::Ok;
}

char *CTokenParser::getNextToken()
{
	if(m_nPos < m_sBufferRows)
		return m_buffer[m_nPos++];
	else
		return nullptr;
}

void CTokenParser::cleanBuffer()
{
	memset(m_buffer, 0, sizeof m_buffer);
}


void CTokenParser::resetPos()
{
	m_nPos = 0;
}

void CTokenParser::clean()
{
	cleanBuffer();
	resetPos();
}

