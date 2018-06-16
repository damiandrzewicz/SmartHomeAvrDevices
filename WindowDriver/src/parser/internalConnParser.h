/*
 * internalConnParser.h
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#pragma once

#include <avr/io.h>
#include "dataParser.h"

class CInternalConnParser {
public:

	CInternalConnParser();
	virtual ~CInternalConnParser();

	void prepareOutputMessage(char *content);
	ParseResult parseInputMessage(char *msg);

	void cleanBuffer();
	char *getBuffer();

private:
	static const uint8_t m_sBufferRows = 55;
	char m_buffer[m_sBufferRows];
};


