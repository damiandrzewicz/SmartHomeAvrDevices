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

#include "servo/servo.h"
#include "encoders/quadratureEncoder.h"
#include "controllers/servoController.h"

#include "controllers/UartController.h"




#define DEVICE_ADDRESS "x0001"

//Encoder variables
extern CQuadratureEncoder encoderForServo1;
extern CQuadratureEncoder encoderForServo2;

CServoController servoController1;
CServoController servoController2;

CUartController uartController;

void uartCallback(char *data);
void timerCallback();


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
	CServo servo1(servoData1, 1);	//CreateServo1 object
	CServo servo2(servoData2, 2);	//CreateServo2 object

	/*
	 * Servo controllers
	 */
	servoController1.registerObjects(&servo1, &encoderForServo1);
	servoController2.registerObjects(&servo2, &encoderForServo2);

	/*
	 * Servo models controller
	 */
	uartController.setServoModel1(servo1.getServoModel());
	uartController.setServoModel2(servo2.getServoModel());

	sei();

	DDRC |= (1 << PC0);
	DDRC |= (1 << PC1);

	//PORTC |= (1 << PC0);
	//PORTC &= ~(1 << PC1);

	//DDRB |= (1 << PB7);

	//DDRC |= (1 << PC0);
	//DDRC |= (1 << PC1);

	DDRB |= (1 << PB7);
	PORTB &= ~(1 << PB7);

	//CUart::getInstance()->puts("Hello123!");			//Send response by UART
	//CUart::getInstance()->puts("\n\r");				//Terminate response


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

	}
}

void uartCallback(char *data)
{
	//CUart::getInstance()->puts("Received Uart:");
	//CUart::getInstance()->puts(data);
	//CUart::getInstance()->puts("\r\n");
	uartController.uartDataReady(data);
}





