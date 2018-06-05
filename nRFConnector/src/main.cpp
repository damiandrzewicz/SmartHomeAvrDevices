/*
 * main.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: damian
 */

#include <avr/io.h>
#include <util/delay.h>

#include "controllers/masterController.h"
#include "nrf/nrf.h"
#include "nrf/nRF24L01_memory_map.h"
#include "spi/SPI.h"
#include "uart/uart.h"
#include "dataParser/dataParser.h"
#include "timer/timer.h"
#include "portability.h"

//bool bflag = false;
//
//void tick()
//{
//	//CUart *uart = CUart::getInstance();
//	//uart->puts("test\n\r");
//
////	DDRD |= (1 << PD4);
////	PORTD &= ~(1 << PD4);
//
//	PORTC ^= (1 << PC0);
//	bflag = true;
//
//	//CUart::getInstance()->puts("dmaian\n\r");
//}



int main()
{
	DDRC |= (1 << PC0);
	//PORTC &= ~(1 << PC5);

	const uint8_t nDeviceAddress = 0x01;

	//Initialise UART interface
	CUart::setBaudRate(__UBRR);								//Default value for baud rate
	CUart *uart = CUart::getInstance();						//Get global instance of UART interface
	uart->registerEventCallback(uartCallback);		//Register uart event callback

	//Get SPI interface
	SPI::getInstance();

	//Initialise nRF24L01
	CNrf *nrf = CNrf::getInstance();

	//Setup radio - default settings
	nrf->setTransmitterAdres(nDeviceAddress);
	nrf->setReciverAddres(RX_ADDR_P0, nDeviceAddress);

	nrf->setStateAndWidthOfCRC( ONE_BYTE , OFF  );
	nrf->setChannel(12);
	nrf->setActiveDataPipeAndACK( ERX_P0, ON, ACK_OFF  );
	nrf->setRetransmissionTimeAndAmmount(WAIT_250uS , RETR_15_TIMES );
	nrf->setDataSpeedAndReciverPower(TRANS_SPEED_2MB, RF_PWR_0dB );
	nrf->setDynamicPayloadStateOnDataPipe( DPL_P0 , ON  );
	nrf->registerRX_Event_Callback(nrfCallback);

	//Start radio
	nrf->RX_PowerUp();

	//Get controller instance and initialise
	CController *controller = CController::getInstance();

	//Initialise timer
	CTimer2 *timer2 = CTimer2::getInstance(CTimer2::T2Prescallers::PS_1024, 77);
	timer2->Assign(controller->getTimerHandle(), 0, timerCallback, false);
	//timer2->Assign(1, 50, tick, true);


   	//Allow for interrupts
    sei();

	while(1)
	{

//		if(bflag)
//		{
//			CUart::getInstance()->puts("dmaian\n\r");
//			bflag = false;
//		}

		//CUart::getInstance()->puts("dmaian\n\r");
		//_delay_ms(500);
		//Uart event
		uart->RX_STR_EVENT();
		nrf->RX_EVENT();

		controller->controllerEvent();
	}
}





