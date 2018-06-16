/*
 * internalConnParser.cpp
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#include <string.h>

#include "internalConnParser.h"
#include "../settings.h"
#include "../uart/uart.h"

CInternalConnParser::CInternalConnParser() {
	// TODO Auto-generated constructor stub

}

CInternalConnParser::~CInternalConnParser() {
	// TODO Auto-generated destructor stub
}

void CInternalConnParser::cleanBuffer()
{
	memset(m_buffer, 0, m_sBufferRows);
}

char *CInternalConnParser::getBuffer()
{
	return m_buffer;
}


void CInternalConnParser::prepareOutputMessage(char *content)
{
	cleanBuffer();

	char temp[6];
	strcpy(temp, "@");
	strcat(temp, UART_OUTPUT_MSG_TYPE);
	strcat(temp, "@");

	//Prepare header
	strcpy(m_buffer, temp);

	//Prepare content
	strcat(m_buffer, content);

	//Finish data
	strcat(m_buffer, "@");
}

ParseResult CInternalConnParser::parseInputMessage(char *msg)
{
	cleanBuffer();

	//Message received, process it
	CDataParser parser;
	ParseResult result = parser.parseData(msg, "@");

	if(result != ParseResult::Ok)
	{
		return result;
	}

	char *cRequest = parser.getNextToken();
	//CUart::getInstance()->puts(cRequest);
	//CUart::getInstance()->puts("\r\n");

	if(strcmp(cRequest, UART_INPUT_MSG_TYPE) != 0)
	{
		return ParseResult::BadRequest;
	}


	//Get data
	char *cData = parser.getNextToken();

	//CUart::getInstance()->puts(cData);
	//CUart::getInstance()->puts("\r\n");

	//Sve data in buffer
	strcpy(m_buffer, cData);

	return ParseResult::Ok;

}

