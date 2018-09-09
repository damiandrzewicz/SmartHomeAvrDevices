//Include main libraries
#include <avr/io.h>

//Include custom libraries
#include "RF24/RF24.h"
#include "spi/SPI.h"
#include "uart/uart.h"
#include "timer/timer.h"
#include "portability.h"
#include "utils/utils.h"
#include "controllers/BaseController.h"

#include "TokenParser/TokenParser.h"

//Callbacks
void uartCallback(char *cUartData);
void nrfCallback(char *cRadioData, uint8_t nDataLen);

bool bSend = false;
bool bmode = false;
void tick()
{
	bSend = true;
}

#if NRF_MODE == 0
	#include "controllers/TerminalController.h"
	CTerminalController controller;
#elif NRF_MODE == 1
#include "controllers/ConnectorController.h"
	//TODO create object
#elif NRF_MODE == 2
#include "controllers/StandaloneDeviceController.h"
	//TODO create object
#endif

int main()
{
	/////////////////////////////////////////////
	//Initialise UART interface
	/////////////////////////////////////////////
	CUart::setBaudRate(__UBRR);								//Default value for baud rate
	CUart *uart = CUart::getInstance();						//Get global instance of UART interface
	uart->registerEventCallback(uartCallback);		//Register uart event callback

	sei();

	//CUart::getInstance()->puts("hello1\r\n");

	/////////////////////////////////////////////
	//Initialise timer 2
	/////////////////////////////////////////////
	CTimer2 *timer2 = CTimer2::getInstance(CTimer2::T2Prescallers::PS_1024, 14);
	timer2->Assign(0, 0, increaseMillis, true);		//This tick is used for timer purposes - time measure
	timer2->Assign(1, 1000, tick, true);
	//CUart::getInstance()->puts("hello2\r\n");

	sei();

	/////////////////////////////////////////////
	//Initialise nRF24l01 connector
	/////////////////////////////////////////////
	RF24 *radio = RF24::getInstance();
	radio->registerCallback(nrfCallback);
	radio->begin();
	radio->setDataRate(rf24_datarate_e::RF24_2MBPS);


	radio->startListening();
	radio->setDeviceAddress(0x0001);

	/////////////////////////////////////////////
	//Allow interrupts
	/////////////////////////////////////////////


	CUart::getInstance()->puts("hello3\r\n");


	//Infinity loop
	while(1)
	{
		//Radio event for processing receiving and sending data
		radio->eventRF24();
		uart->RX_STR_EVENT();
		controller.eventLoop();
	}
}

/////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////
void uartCallback(char *cUartData)
{
	controller.uartDataReady(cUartData);
}

void nrfCallback(char *cRadioData, uint8_t nDataLen)
{
	//CUart::getInstance()->puts("here\r\n");
	controller.radioDataReady(cRadioData);
}
