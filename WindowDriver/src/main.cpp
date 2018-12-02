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
#include "uart/uart.h"
#include "utils/utils.h"
#include "settings.h"

#include "servo/servo.h"
#include "encoders/quadratureEncoder.h"
#include "controllers/servoController.h"

#include "controllers/UartController.h"



//Encoder variables
extern CQuadratureEncoder encoderForServo1;
extern CQuadratureEncoder encoderForServo2;

CServoController servoController1;
CServoController servoController2;

CUartController uartController;

void uartCallback(char *data);
//void timerCallback();

bool s = false;
void flag()
{
	s = true;
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
	CTimer2 *timer2 = CTimer2::getInstance(CTimer2::T2Prescallers::PS_1024, 14);
	timer2->Assign(0, 0, increaseMillis, true);

	timer2->Assign(1, 500, flag, true);

	sei();

	/*
	 * *********************
	 * ServoData structures
	 * *********************
	 */
	//Prepare servo1 data
	ServoData servoData1;
	servoData1.pinM1 = { &DDRC, &PORTC, &PINC, PC0, };
	servoData1.pinM2 = { &DDRC, &PORTC, &PINC, PC1, };
	servoData1.pinADC = { &DDRA, &PORTA, &PINA, PA0, };
	servoData1.adcMaxCurrent = 380;			//mA
	servoData1.adcResistorValue = 470;		//mOhm
	servoData1.enablePwmFunPtr = &CTimer1::setChannelA;
	servoData1.timer1Obj = timer1;

	//timer1->setChannelA(255);
//
//	//Prepare servo2 data
	ServoData servoData2;
	servoData2.pinM1 = { &DDRC, &PORTC, &PINC, PC2, };
	servoData2.pinM2 = { &DDRC, &PORTC, &PINC, PC3, };
	servoData2.pinADC = { &DDRA, &PORTA, &PINA, PA1, };
	servoData2.adcMaxCurrent = 300;			//mA
	servoData1.adcResistorValue = 1100;		//mOhm
	servoData2.enablePwmFunPtr = &CTimer1::setChannelB;
	servoData2.timer1Obj = timer1;

	//timer1->setChannelB(100);
//
//	//Servo models
	CServoModel servoModel_1(1);
	CServoModel servoModel_2(2);

	/*
	 * **********
	 * CServo
	 * **********
	 */
	//CServo servo1(servoData1, 1);	//CreateServo1 object
	//CServo servo2(servoData2, 2);	//CreateServo2 object

	/*
	 * Servo controllers
	 */
	servoController1.registerObjects(&servoData1, &encoderForServo1, &servoModel_1);
	servoController2.registerObjects(&servoData2, &encoderForServo2, &servoModel_2);


	/*
	 * Servo models controller
	 */
	uartController.setServoModel1(&servoModel_1);
	uartController.setServoModel2(&servoModel_2);



	//uartController.setConnectorAddress(DEVICE_ADDRESS);

	//DDRC |= (1 << PC0);
	//DDRC |= (1 << PC1);

	//PORTC |= (1 << PC0);
	//PORTC &= ~(1 << PC1);

	//DDRB |= (1 << PB7);

	//DDRC |= (1 << PC0);
	//DDRC |= (1 << PC1);

	DDRB |= (1 << PB7);
	PORTB &= ~(1 << PB7);

	DDRB |= (1 << PB5);
	PORTB &= ~(1 << PB5);

	//CUart::getInstance()->puts("Hello123!");			//Send response by UART
	//CUart::getInstance()->puts("\n\r");				//Terminate response

	//DDRD &= ~(1 << PD0);
	//PORTD |= (1 << PD0);

	servoController1.startGointToInitPosition();

	while(1)
	{
		//CUart::getInstance()->puts("damian\r");

		//_delay_ms(1000);

		//Uart event
		uart->RX_STR_EVENT();

		//Uart parser event
		uartController.loopEvent();

		//Servo events
		servoController1.event();
		//servoController1.event();

//		if(!s && servoModel_1.getManualDriveDirection() != WindowData::Direction::Stop)
//		{
//			CUart::getInstance()->puts("changed=");
//			s=true;
//		}

//		if( !(PIND & (1 << PD0)) )
//		{
//			PORTB ^= (1 << PB5);
//		}
//
		if(s)
		{
			CUart::getInstance()->puts("enc=");
			CUart::getInstance()->putll(encoderForServo1.getCounter(), 10);
			CUart::getInstance()->puts("\r\n");

			//CUart::getInstance()->puts("curr=");
			//CUart::getInstance()->putll(servoController1.current, 10);
			//CUart::getInstance()->puts("\r\n");

			s = false;
		}

	}
}

void uartCallback(char *data)
{
//	CUart::getInstance()->puts("Received Uart:");
//	CUart::getInstance()->puts(data);
//	CUart::getInstance()->puts("\r\n");
	uartController.uartDataReady(data);
}





