/*
 * CDataParser.cpp
 *
 *  Created on: Apr 7, 2018
 *      Author: damian
 */

#include <string.h>
#include <stdlib.h>
#include "dataParser.h"

#include "../uart/uart.h"

CDataParser::CDataParser() {
	// TODO Auto-generated constructor stub

}

CDataParser::~CDataParser() {
	// TODO Auto-generated destructor stub
}

CDataParser::ParseResult CDataParser::parseData(char *data)
{
	//Reset pos
	resetPos();

	//Get size
	uint8_t len = strlen(data);

	//Check data object
	if(data == nullptr)
		return ParseResult::NullDataPointer;
	else if(len == 0)
		return ParseResult::EmptyDataPointer;

	//Check first character
	if(data[0] != '@')
		return ParseResult::BadFirstChar;

	//Check last character
	if(data[len-1] != '@')
		return ParseResult::BadLastChar;

	//Get request
	m_buffer[0] = strtok(data, "@");
	if(strcmp(m_buffer[0], "") == 0)
		return ParseResult::EmptyRequest;

	//Get operation
	m_buffer[1] = strtok(NULL, "@");
	if(strcmp(m_buffer[1], "") == 0)
		return ParseResult::EmptyOperation;

	//Get value
	m_buffer[2] = strtok(NULL, "@");
	if(strcmp(m_buffer[2], "") == 0)
		return ParseResult::EmptyValue;

	return ParseResult::Ok;
}

void CDataParser::cleanBuffer()
{
	//memset(m_buffer, 0, sizeof(m_buffer[0][0]) * m_sBufferRows);
	//Clean pointers
	uint8_t i;
	for(i = 0; i < m_sBufferRows; ++i)
	{
		m_buffer[i] = nullptr;
	}
}

char *CDataParser::getNextToken()
{
	if(m_nPos < m_sBufferRows)
		return m_buffer[m_nPos++];
	else
		return nullptr;
}

void CDataParser::resetPos()
{
	m_nPos = 0;
}


