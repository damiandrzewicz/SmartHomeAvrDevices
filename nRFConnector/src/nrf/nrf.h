/*
 * nrf.h
 *
 *  Created on: Mar 24, 2018
 *      Author: damian
 */

#ifndef NRF_NRF_H_
#define NRF_NRF_H_

#include "../config.h"

#include "../io/io.h"
#include "../spi/ioSettings.h"
#include "../spi/SPI.h"
#include "ioSettings.h"
#include "nrfSettings.h"

#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

extern "C" void INT0_vect (void) __attribute__ ((signal,used, externally_visible)) ;

class CNrf {
	public:
	static CNrf *getInstance();

	void RX_EVENT (void);			      //this function checks if there are data to read and if there is callback function registered.
											  //if there are data avaliable to read this function sends this data to registered calbback function
	void registerRX_Event_Callback ( void ( * callback )( void * nRF_RX_buff , uint8_t len ) ); //this function is used to register RX_EVENT callback function

	void sendDataToAir( char * data ); //this function sends data to to another nRF or nRF's. Length of data payload is calculeted by strlen funcion so
											  //you must send data in ASCII format
	uint8_t dataReady(void);			  //this function is used when IRQ are disabled. It checks if there any data to read
	void initializeInterrupt(void);  //this function is used to initlialization of external interrupt for nRF
	void configRegister(uint8_t register_name, uint8_t value);        //WRITE SOMETHING TO NRF REGISTER
	void setTransmitterAdres(const char * addres);				   //this function sets transmitter addres
	void setTransmitterAdres(uint16_t address);
	void setReciverAddres( uint8_t data_pipe, const char * addres ); //this function sets reciver addres of choosen datapipe
	void setReciverAddres( uint8_t data_pipe, uint16_t address);
	uint8_t readOneByteFromRegister(uint8_t register_name);		   //this function returns value of register given by attribute
	void setRetransmissionTimeAndAmmount (uint8_t time, uint8_t ammount); //this function sets time beetwen retransmissions and ammount of them
	void TX_PowerUp(void);				  //this function turns on TX mode
	void RX_PowerUp(void);				  //this function turns on RX mode
	void powerDown(void);			      //this funtion turns on Power_down mode
	void setChannel( uint8_t channel );  //this function sets channel ( 0 to 127 )
	void clear_TX (void);				  //this function clears TX fifo
	void clear_RX (void);				  //this function clears RX fifo

	void intToAddressArray(uint8_t value, char *buf);

	void printNrfInfo();

	friend void INT0_vect (void);

private:
	//Cnstructors
	CNrf();	//Prevent consttuction but allow in getInstance
	CNrf(const CNrf&);	//Prevent construction by copying
	CNrf& operator=(const CNrf&);	//Prevent assigment
	~CNrf();					//Prevent unwanted destruction

	void init();
	void setupPins();

	//// callback function declaration
	void ( * nRF_RX_Event_Callback ) ( void * nRF_RX_buff , uint8_t len );

	////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	//// declarations of internal tool functions
	//..................................................................................................................
	void readRegisters( uint8_t register_name, uint8_t * buffer_for_registers_content, uint8_t length_of_content );  //reads given ammount of bytes from choosen register
	void writeRegister( uint8_t register_name, uint8_t * buffer_for_registers_content, uint8_t length_of_content ); //writes given ammount of bytes to choosen register

public:
	void setActiveDataPipeAndACK (uint8_t DataPipe, uint8_t on_or_off, uint8_t ACK_on_or_off );					 //This function is used to set active payload/payloads and enable/disable ACK functions
	void setPayloadWidth ( uint8_t payload, uint8_t width );  													 //this function is used to set payload length. Use this function if dynamic payload length is disabled
	void setDynamicPayloadStateOnDataPipe(uint8_t data_pipe_number, uint8_t on_off ); 						 //this function is used to set
	void setDataSpeedAndReciverPower(uint8_t Data_rate, uint8_t power);                                         //this function is used to set transmission speed and reciver power
	void setStateAndWidthOfCRC( uint8_t one_or_two_bytes , uint8_t on_or_off);                                  //this function is used to set CRC state and width.

private:
	static CNrf * m_sNrfInstance;
	static bool m_bIsCreated;

	//// declaration of global variables which stores values depending on the transmission states
	volatile uint8_t m_TX_flag;   //data sent event flag
	volatile uint8_t m_RX_flag;   //data ready event flag

	volatile uint8_t m_disableDynamicPayload;
	volatile uint8_t m_payloadWidth;
	volatile uint8_t m_firstTime = 0;
	volatile uint8_t m_setFirstTimeIntoRXmodeAfterTXmode = 0;

	//// declaration of the buffer which stores incoming payloads data
	uint8_t m_RX_bufffer[MAXIMUM_PAYLOAD_SIZE+1]; //reciver buffer

	// Spi object
	SPI *m_spi;
};

#endif /* NRF_NRF_H_ */
