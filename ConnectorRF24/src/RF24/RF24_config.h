
/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
 /* spaniakos <spaniakos@gmail.com>
  Added __ARDUINO_X86__ support
*/

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__

#include <avr/io.h>
#include "../spi/ioSettings.h"

/*** USER DEFINES:  ***/
//#define FAILURE_HANDLING
//#define SERIAL_DEBUG
//#define MINIMAL
//#define SPI_UART  // Requires library from https://github.com/TMRh20/Sketches/tree/master/SPI_UART
//#define SOFTSPI   // Requires library from https://github.com/greiman/DigitalIO

/**********************/

#define rf24_max(a,b) (a>b?a:b)
#define rf24_min(a,b) (a<b?a:b)
#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

#define MAXIMUM_PAYLOAD_SIZE 32
#define DEFAULT_ADDRESS "9999"

#define IRQ 	PD2			//IRQ PIN

#define IRQ_DDR_PORT DDRD	//direction port
#define IRQ_PORT     PORTD	//output port
#define IRQ_PIN      PIND	//input port

#define CE  	PB0		//CE PIN

#define CE_DDR_PORT DDRB	//direction port
#define CE_PORT     PORTB	//output port
#define CE_PIN      PINB	//input port

#define CSN 	PB2		//Chip select PIN

#define CSN_DDR_PORT DDRB	//direction port
#define CSN_PORT     PORTB	//output port
#define CSN_PIN      PINB	//input port

#define NRF_DDR_PORT DDRC	//direction register of port which control nRF power
#define NRF_PORT            PORTC	//output register of port which control nRF power
#define NRF_PIN             PC0		//pin on the output register thah controls nRF power

#define NRF_ON 	NRF_PORT &= ~(1<<NRF_PIN);	//turn on nRF
#define NRF_OFF NRF_PORT |= (1<<NRF_PIN);	//turn off nRF

#define CSN_HIGH CSN_PORT |=  (1<<CSN) 	//make CSN high macro
#define CSN_LOW  CSN_PORT &= ~(1<<CSN)	//make CSN low macro

#define CE_HIGH  CE_PORT |=  (1<<CE)	//make CE high macro
#define CE_LOW   CE_PORT &= ~(1<<CE)	//make CE low macro

#define POWER_DDR DDRD
#define POWER_PORT PORTD
#define POWER_PIN PD3

#endif // __RF24_CONFIG_H__

