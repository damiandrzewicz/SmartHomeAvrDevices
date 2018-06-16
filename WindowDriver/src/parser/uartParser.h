/*
 * uartParser.h
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#ifndef SRC_CONTROLLERS_UARTPARSER_H_
#define SRC_CONTROLLERS_UARTPARSER_H_

#include "dataParser.h"
#include "../servo/servo.h"

class CUartParser {
public:
	static CUartParser * getInstance();
	void registerServo(CServo *srv1, CServo *srv2);

	void setDataBuffer(char *buffer);
	void clearResponseBuffer();

	//Setters
	void setReadyForProcessData(bool value);
	void setReadyForProcessResponse(bool value);
	void setResponseBuffer(char *data);

	//Getters
	bool isReadyForProcessData();
	bool isReadyForProcessResponse();

	//Operation process
	bool processSayHello(CDataParser *data);
	bool processGetBlindType(CDataParser *data);
	bool processGetBlindState(CDataParser *data);
	bool processSetblindState(CDataParser *data);
	bool processCalibrate(CDataParser *data);
	bool processSetBlindType(CDataParser *data);

	void prepareCallbackMsg(char *buff);
	void sendCallbackMsg();

	void setError(bool value);

	void processStepTransition(uint8_t step);
	void clearStepTransition();

	void event();

	//Operaions
private:
	//Cnstructors
	CUartParser();	//Prevent consttuction but allow in getInstance
	CUartParser(const CUartParser&);	//Prevent construction by copying
	CUartParser& operator=(const CUartParser&);	//Prevent assigment
	~CUartParser();					//Prevent unwanted destruction

	//Members
private:
	static CUartParser * s_instance;
	static bool s_bIsCreated;

	bool m_bReadyRofProcessData = false;
	bool m_breadyForProcessResponse = false;

	//CDataParser m_dataFromAir;

	char * m_buffer = nullptr;
	char m_responseBuffer[15];

	bool m_bError = false;

	uint8_t test = 0;

	CServo *m_pServo1 = nullptr;
	CServo *m_pServo2 = nullptr;


};

#endif /* SRC_CONTROLLERS_UARTPARSER_H_ */
