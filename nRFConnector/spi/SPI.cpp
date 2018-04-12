/*
 * SPI.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: damian
 */

#include <util/delay.h>
#include <avr/interrupt.h>
#include "SPI.h"
#include "ioSettings.h"


SPI *SPI::m_sSpiInstance = nullptr;
bool SPI::m_bIsCreated = false;

SPI *SPI::getInstance()
{
	if(!m_bIsCreated)
	{
		static SPI timer;
		m_sSpiInstance = &timer;
		m_bIsCreated = true;
	}
	return m_sSpiInstance;
}

SPI::SPI() {
	init();
}

SPI::~SPI() {
}

#if USE_SOFTWARE_SPI == 0

void SPI::init()
{
//	CE_DDR_PORT |= (1<<CE);
//	CSN_DDR_PORT |= (1<<CSN);
//	SS_DDR_PORT |= (1<<SS);
//	MOSI_DDR_PORT |= (1<<MOSI);
//	MISO_DDR_PORT &= ~(1<<MISO);
//	SCK_DDR_PORT |= (1<<SCK);
//
//	CE_PORT  &= ~(1<<CE);
//	CSN_PORT |= (1<<CSN);
//	SS_PORT  |= (1<<SS);
//	MOSI_PORT &= ~(1<<MOSI);
//	MISO_PORT &= ~(1<<MISO);
//	SCK_PORT &= ~(1<<SCK);

	//SET pins MOSI, CSN(chip select),SCK, i CE(control RX, TX nRFA) as ouputs
		SPCR |= (0<<SPIE) | //turn OFF SPI intterrupt
				(1<<SPE)  | //enable SPI module
				(0<<DORD) | //set data order
				(1<<MSTR) | //enable SPI in master mode
				(0<<CPOL) | //SCK polarity, rising edge is active
				(0<<CPHA) | //data polarity, read data on rising edge
				(0<<SPR0) | //SCK speed
				(0<<SPR1) ; //
		SPSR |= (1<<SPI2X); //SPI sck 2 times faster
}

uint8_t SPI::writeReadByte(uint8_t data)
{
	SPDR = data;          		 //send byte to SPDR register
	while(!(SPSR & (1<<SPIF)));  //wait for data shifting
	return SPDR;
}

void SPI::writeByte(uint8_t data)
{
	SPDR = data;          		//send byte to SPDR register
	while(!(SPSR & (1<<SPIF))); //wait for data shifting
}

void SPI::writeDataBuffer(uint8_t *dataBuferOut, uint8_t length)
{
	uint8_t * pointer;				//declare pointer
	uint8_t i;						//declare indexing variable
	pointer = dataBuferOut;		//pointer is showing on first byte of "data_buffer_out"
	for( i = 0; i < length; i++ )	//until "i" is smaller than "length"
	{
		writeByte( *pointer++ );//send bytes to nRF
	}
}

void SPI::writeReadDataBuffer(uint8_t *dataBufferIn, uint8_t *dataBufferOut, uint8_t length)
{
	uint8_t * pointer1;				//declare pointer1
	uint8_t * pointer2;				//declare pointer2
	uint8_t i;						//declare indexing variable

	pointer1 = dataBufferOut;		//pointer1 is showing on first byte of "data_buffer_out"
	pointer2 = dataBufferIn;		//pointer2 is showing on first byte of "data_buffer_in"
	for( i = 0; i < length; i++ )	//until "i" is smaller than "length"
	{
		*pointer2++ = writeReadByte( *pointer1++ ); //send bytes to nRF and read data back from nRF
	}
}

#endif

#if USE_SOFTWARE_SPI == 1

void SPI::init()
{
//	CE_DDR_PORT |= (1<<CE);
//	CSN_DDR_PORT |= (1<<CSN);
//	SS_DDR_PORT |= (1<<SS);
//	MOSI_DDR_PORT |= (1<<MOSI);
//	MISO_DDR_PORT &= ~(1<<MISO);
//	SCK_DDR_PORT |= (1<<SCK);
//
//	CE_PORT  &= ~(1<<CE);
//	CSN_PORT |= (1<<CSN);
//	SS_PORT  |= (1<<SS);
//	MOSI_PORT &= ~(1<<MOSI);
//	MISO_PORT &= ~(1<<MISO);
//	SCK_PORT &= ~(1<<SCK);
//
//	_delay_ms(10);
}

uint8_t SPI::writeReadByte(uint8_t data)
{
	register uint8_t i;									//indexing variable "i"

	for( i = 0; i < 8 ; i++ )							//until "i" is smaller than 8
	{
		if( data & 0b10000000 )							// if MSB of byte is equal to one
		{
			MOSI_PORT |= (1<<MOSI);						// MOSI=1;
		}
		else											//else
		{
			MOSI_PORT &= ~(1<<MOSI);						// MOSI=0;
		}

		data = ( data << 1 );           				//shift next bit into MSB

		if( MISO_PIN & (1<<MISO) ) data |= 1;			//if MISO pin is HIGH set LSB of data to ONE
		SCK_PORT |= (1<<SCK);							//make SCK pin high
#if F_CPU > 16000000									//if F_CPU is biger than 16000000
		asm("nop");										//put here asm("nop") command
#endif
		SCK_PORT &= ~(1<<SCK);							//make sck pin low

	}
	return data;
}

void SPI::writeByte(uint8_t data)
{
	register uint8_t i;								//indexing variable "i"
	for( i = 0; i < 8 ; i++ )						//until "i" is smaller than 8
	{

		if( data & 0b10000000 )						// if MSB of byte is equal to one
		{
			MOSI_PORT |= (1<<MOSI);					// MOSI=1;
		}
		else										//else
		{
			MOSI_PORT &= ~(1<<MOSI);					//MOSI=0;
		}

		data = ( data << 1 );						//shift data to left
		SCK_PORT |= (1<<SCK);						//SCK high
#if F_CPU > 16000000									//if F_CPU is biger than 16000000
	asm("nop");										//put here asm("nop") command
#endif
		SCK_PORT &= ~(1<<SCK);						//SCK low
	}
}

void SPI::writeDataBuffer(uint8_t *dataBuferOut, uint8_t length)
{
	uint8_t * pointer;				//declare pointer
	uint8_t i;						//declare indexing variable
	pointer = dataBuferOut;		//pointer is showing on first byte of "data_buffer_out"
	for( i = 0; i < length; i++ )	//until "i" is smaller than "length"
	{
		writeByte( *pointer++ );//send bytes to nRF
	}
}

void SPI::writeReadDataBuffer(uint8_t *dataBufferIn, uint8_t *dataBufferOut, uint8_t length)
{
	uint8_t * pointer1;				//declare pointer1
	uint8_t * pointer2;				//declare pointer2
	uint8_t i;						//declare indexing variable

	pointer1 = dataBufferOut;		//pointer1 is showing on first byte of "data_buffer_out"
	pointer2 = dataBufferIn;		//pointer2 is showing on first byte of "data_buffer_in"
	for( i = 0; i < length; i++ )	//until "i" is smaller than "length"
	{
		*pointer2++ = writeReadByte( *pointer1++ ); //send bytes to nRF and read data back from nRF
	}
}

#endif


