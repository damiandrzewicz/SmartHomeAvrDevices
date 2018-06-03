/*
 * ioSettings.h
 *
 *  Created on: Mar 24, 2018
 *      Author: damian
 */

#ifndef NRF_IOSETTINGS_H_
#define NRF_IOSETTINGS_H_

#include <avr/io.h>


#define IRQ 	PD2			//IRQ PIN

#define IRQ_DDR_PORT DDRD	//direction port
#define IRQ_PORT     PORTD	//output port
#define IRQ_PIN      PIND	//input port


#define NRF_DDR_PORT DDRC	//direction register of port which control nRF power
#define NRF_PORT            PORTC	//output register of port which control nRF power
#define NRF_PIN             PC0		//pin on the output register thah controls nRF power

#define NRF_ON 	NRF_PORT &= ~(1<<NRF_PIN);	//turn on nRF
#define NRF_OFF NRF_PORT |= (1<<NRF_PIN);	//turn off nRF


#define CSN_HIGH CSN_PORT |=  (1<<CSN) 	//make CSN high macro
#define CSN_LOW  CSN_PORT &= ~(1<<CSN)	//make CSN low macro

#define CE_HIGH  CE_PORT |=  (1<<CE)	//make CE high macro
#define CE_LOW   CE_PORT &= ~(1<<CE)	//make CE low macro


#endif /* NRF_IOSETTINGS_H_ */
