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

//CDataParser *CDataParser::m_sInstance = nullptr;
//bool CDataParser::m_bIsCreated = false;
//
//CDataParser *CDataParser::getInstance()
//{
//	if(!m_bIsCreated)
//	{
//		static CDataParser object;
//		m_sInstance = &object;
//		m_bIsCreated = true;
//	}
//	return m_sInstance;
//}

CDataParser::CDataParser() {
	// TODO Auto-generated constructor stub

}

CDataParser::~CDataParser() {
	// TODO Auto-generated destructor stub
}

int8_t CDataParser::parseData(char *data)
{

	//Reset pos
	resetPos();

	//Get size
	uint8_t len = strlen(data);

	//Check data object
	if(data == nullptr)
		return -1;
	else if(len == 0)
		return -2;

	//Check first character
	if(data[0] != '@')
		return -3;

	//Check last character
	if(data[len-1] != '@')
		return -4;

	//Dummy value
	//strtok(data, "@");

	//Get request
	m_buffer[0] = strtok(data, "@");
	if(strcmp(m_buffer[0], "") == 0)
		return -5;

	//Get operation
	m_buffer[1] = strtok(NULL, "@");
	if(strcmp(m_buffer[1], "") == 0)
		return -6;


	//Get value
	m_buffer[2] = strtok(NULL, "@");
	if(strcmp(m_buffer[2], "") == 0)
		return -7;


	return 1;

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


