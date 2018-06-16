/*
 * main.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: damian
 */

#include <avr/io.h>
#include <util/delay.h>
#include "avr/interrupt.h"
#include "portability.h"
#include "timer/timer.h"
#include "io/io.h"

#include "servo/servo.h"
#include "encoders/quadratureEncoder.h"
#include "parser/uartParser.h"

#include "uart/uart.h"

#define DEVICE_ADDRESS "x0001"

//Encoder variables
extern CQuadratureEncoder encoderForServo1;
extern CQuadratureEncoder encoderForServo2;

volatile bool sendflag = false;
volatile int8_t step = 0;
volatile bool ready = false;

bool senduart = false;

void uartCallback(char *data);

void tick()
{
	if(!sendflag)
	{

		sendflag = true;
	}
}

void hehe()
{
	static uint8_t tick = 0;
	if(tick < 5)
	{
		tick++;
		if(tick == 4)
			ready = true;
	}
}


int main()
{

	//Initialise UART interface singleton
	CUart::setBaudRate(__UBRR);								//Default value for baud rate
	CUart *uart = CUart::getInstance();						//Get global instance of UART interface
	uart->registerEventCallback(uartCallback);		//Register uart event callback

	//Initialise timer1 singleton
	CTimer1 * timer1 = CTimer1::getInstance();		//Timer1 with PWM mode - 8bit, non inverse

	//Initialise timer2 singleton
	CTimer2 *timer2 = CTimer2::getInstance(CTimer2::T2Prescallers::PS_1024, 155);
	//timer2->Assign(controller->getTimerHandle(), 0, timerCallback, false);
	timer2->Assign(1, 10, tick, true);
	timer2->Assign(2, 100, hehe, true);

	/*
	 * *********************
	 * ServoData structures
	 * *********************
	 */
	//Prepare servo1 data
	ServoData servoData1;
	servoData1.pinM1 = { &DDRC, &PORTC, &PINC, PC0, };
	servoData1.pinM2 = { &DDRC, &PORTC, &PINC, PC1, };
	servoData1.enablePwmFunPtr = &CTimer1::setChannelA;
	servoData1.timer1Obj = timer1;

	//Prepare servo2 data
	ServoData servoData2;
	servoData2.pinM1 = { &DDRC, &PORTC, &PINC, PC2, };
	servoData2.pinM2 = { &DDRC, &PORTC, &PINC, PC3, };
	servoData2.enablePwmFunPtr = &CTimer1::setChannelB;
	servoData2.timer1Obj = timer1;

	/*
	 * **********
	 * CServo
	 * **********
	 */
	CServo servo1(servoData1);	//CreateServo1 object
	CServo servo2(servoData2);	//CreateServo2 object

	/*
	 * ***********
	 * CUartParser
	 * ***********
	 */
	CUartParser *uartParser = CUartParser::getInstance();	//Create uart parser
	uartParser->registerServo(&servo1, &servo2);


	sei();

	DDRB |= (1 << PB7);

	DDRC |= (1 << PC0);
	DDRC |= (1 << PC1);


	while(1)
	{
		//Uart event
		uart->RX_STR_EVENT();

		//Uart parser event
		uartParser->event();

		//_delay_ms(1000);



		char buf[4];

		if(sendflag)
		{
			//itoa(encoderForServo1.getCounter(), buf, 10);
			//uart->puts("value: ");
			//uart->puts(buf);
			//uart->puts("\n\r");
			sendflag = false;

		}

		if(ready)
		{
			ready = false;

			//PORTC &= ~ (1 << PC0);
			//PORTC |=  (1 << PC1);


			///_delay_ms(10);
			//PORTC &= ~(1 << PC0);
			//PORTC &= ~(1 << PC1);
			//timer1->setChannelB(0);

		}

		if(senduart)
		{
			//CUart::getInstance()->puts("hehe xD\n\r");
			senduart = false;
		}
	}
}

void uartCallback(char *data)
{

//	CUart::getInstance()->puts(data);
//	CUart::getInstance()->puts("\r\n");
	CUartParser::getInstance()->setDataBuffer(data);
	CUartParser::getInstance()->processStepTransition(1);
}




