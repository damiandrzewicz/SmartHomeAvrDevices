/*
 * ioSettings.h
 *
 *  Created on: Mar 23, 2018
 *      Author: damian
 */

#ifndef SPI_IOSETTINGS_H_
#define SPI_IOSETTINGS_H_

#define CE  	PB0		//CE PIN
#define CSN 	PB2		//Chip select PIN
#define SS 		PB2			//SS set the same as CSN
#define MOSI 	PB3			//MOSI pin
#define MISO 	PB4			//MISO pin
#define SCK 	PB5			//SCK pin

#define CE_DDR_PORT DDRB	//direction port
#define CE_PORT     PORTB	//output port
#define CE_PIN      PINB	//input port

#define CSN_DDR_PORT DDRB	//direction port
#define CSN_PORT     PORTB	//output port
#define CSN_PIN      PINB	//input port

#define SS_DDR_PORT DDRB	//direction port
#define SS_PORT     PORTB	//output port
#define SS_PIN      PINB	//input port

#define MOSI_DDR_PORT DDRB	//direction port
#define MOSI_PORT     PORTB	//output port
#define MOSI_PIN      PINB	//input port

#define MISO_DDR_PORT DDRB	//direction port
#define MISO_PORT     PORTB	//output port
#define MISO_PIN      PINB	//input port

#define SCK_DDR_PORT DDRB	//direction port
#define SCK_PORT     PORTB	//output port
#define SCK_PIN      PINB	//input port


#endif /* SPI_IOSETTINGS_H_ */
