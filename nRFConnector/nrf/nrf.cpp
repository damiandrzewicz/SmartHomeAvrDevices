/*
 * nrf.cpp
 *
 *  Created on: Mar 24, 2018
 *      Author: damian
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "nrf.h"
#include "nRF24L01_memory_map.h"

#include "../uart/uart.h"

CNrf * CNrf::m_sNrfInstance = nullptr;
bool CNrf::m_bIsCreated = false;

CNrf *CNrf::getInstance()
{
	if(!m_bIsCreated)
	{
		static CNrf timer;
		m_sNrfInstance = &timer;
		m_bIsCreated = true;
	}
	return m_sNrfInstance;
}

CNrf::CNrf() {
	// Init nRF24l01
	init();
}

CNrf::~CNrf() {
}

void CNrf::init()
{
	// Set nrf pins
	setupPins();

	//Get Spi interface
	m_spi = SPI::getInstance();

	#if USE_IRQ == 1
	initializeInterrupt(); //initialize INTx or PCINTx interrupt for nRF, you must do it on your Own
	#endif

	//setTransmitterAdres(PSTR("1Node"));								//set transmitter addres
	//setReciverAddres(RX_ADDR_P0, PSTR("1Node"));						//set reciving addres for datapipe0
	configRegister( CONFIG, nRF24L01_CONFIG);         					//Write interrupt masks nRF_CONFIG to CONGIG register of nRF

	clear_RX();                                       				    //Clear RX FIFO
	clear_TX();                                        				    //Clear TX FIFO
	//setStateAndWidthOfCRC( ONE_BYTE , OFF  );        			    //ON == Enable CRC; OFF == disable CRC, ONE_BYTE or TWO_BYTES - width of CRC;
	//setChannel(10);                                   				    //Set channel number
	//setActiveDataPipeAndACK( ERX_P0, ON, ACK_OFF  );				//Set which datapipe state you want to change, in this case datapipe = 0 (ERX_P0), ON - means enable this datapipe, ACK_ON means to enable ACK for choosen data pipe.
	//setActiveDataPipeAndACK( ERX_P1, ON, ACK_OFF  );
	//setRetransmissionTimeAndAmmount(WAIT_250uS , RETR_15_TIMES );  //set time between retransmissions and ammount of retranssmisions
	//setDataSpeedAndReciverPower(TRANS_SPEED_2MB, RF_PWR_0dB );     //Set transmision speed and reciver power
	//setDynamicPayloadStateOnDataPipe( DPL_P0 , ON  );              //Enable dynamic payload of choosen datapipe
	//setDynamicPayloadStateOnDataPipe( DPL_P1 , OFF  );

	m_RX_flag = 1;		//Clear RX flag
	m_TX_flag = 0;        //Clear TX flag
	configRegister(STATUS, (1<<TX_DS) | (1<<RX_DR) | (1<<MAX_RT));    //Clear interrupt bits in STATUS register
}

void CNrf::printNrfInfo()
{
#if (DEBUG_MODE)
	char bufor[8] = {0};

	CUart *uart = CUart::getInstance();
	uart->puts_P(PSTR("\n\r\n\r**********************************************"));
	uart->puts_P(PSTR("\n\r***************nRF24l01 settings**************"));
	uart->puts_P(PSTR("\n\r**********************************************\n\r"));
	uint8_t status = readOneByteFromRegister(STATUS);
	uart->puts_P(PSTR("STATUS = "));
	itoa(status ,bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" RX_DR="));
	uart->putc((status & RX_DR)?'1':'0');
	uart->puts_P(PSTR(" TX_DS="));
	uart->putc((status & TX_DS)?'1':'0');
	uart->puts_P(PSTR(" MAX_RT="));
	uart->putc((status & MAX_RT)?'1':'0');
	uart->puts_P(PSTR(" RX_P_NO="));
	uint8_t rxp = (status >> RX_P_NO) & 0x07;
	itoa(rxp, bufor ,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" TX_FULL="));
	uart->putc((status & TX_FULL)?'1':'0');

	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("RX_ADDR_P0-1 = "));
	itoa(readOneByteFromRegister(RX_ADDR_P0),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_ADDR_P1),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("RX_ADDR_P2-5 = "));
	itoa(readOneByteFromRegister(RX_ADDR_P2),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_ADDR_P3),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_ADDR_P4),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_ADDR_P5),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("TX_ADDR = "));
	itoa(readOneByteFromRegister(TX_ADDR),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("RX_PW_P0-6 = "));
	itoa(readOneByteFromRegister(RX_PW_P0),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_PW_P1),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_PW_P2),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_PW_P3),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_PW_P4),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR(" "));
	itoa(readOneByteFromRegister(RX_PW_P5),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));


	uart->puts_P(PSTR("EN_AA = "));
	itoa(readOneByteFromRegister(EN__AA),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("EN_RXADDR = "));
	itoa(readOneByteFromRegister(EN_RXADDR),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("RF_CH = "));
	itoa(readOneByteFromRegister(RF_CH),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("RF_SETUP = "));
	itoa(readOneByteFromRegister(RF_SETUP),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("CONFIG = "));
	itoa(readOneByteFromRegister(CONFIG),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("DYNPD = "));
	itoa(readOneByteFromRegister(DYNPD),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));

	uart->puts_P(PSTR("FEATURE = "));
	itoa(readOneByteFromRegister(FEATURE),bufor,16);
	uart->puts(bufor);
	uart->puts_P(PSTR("\n\r"));
#endif
}

void CNrf::intToAddressArray(uint8_t value, char *buf)
{
	if (value <= 0xFFFF)
	{
		buf[0] = 'x';
		buf[1] = TO_HEX(((value & 0xF000) >> 12));
		buf[2] = TO_HEX(((value & 0x0F00) >> 8));
		buf[3] = TO_HEX(((value & 0x00F0) >> 4));
		buf[4] = TO_HEX((value & 0x000F));
		buf[5] = '\0';
	}
}

void CNrf::setupPins()
{
	IRQ_DDR_PORT &= ~(1<<IRQ);
	CE_DDR_PORT |= (1<<CE);
	CSN_DDR_PORT |= (1<<CSN);
	SS_DDR_PORT |= (1<<SS);
	MOSI_DDR_PORT |= (1<<MOSI);
	MISO_DDR_PORT &= ~(1<<MISO);
	SCK_DDR_PORT |= (1<<SCK);

	IRQ_PORT |= (1<<IRQ);
	CE_PORT  &= ~(1<<CE);
	CSN_PORT |= (1<<CSN);
	SS_PORT  |= (1<<SS);
	MOSI_PORT &= ~(1<<MOSI);
	MISO_PORT &= ~(1<<MISO);
	SCK_PORT &= ~(1<<SCK);
}

void CNrf::RX_EVENT (void)
{
#if USE_IRQ == 0
            if ( dataReady() )  m_RX_flag = 1;
            else m_RX_flag = 0;
    #endif
            if(m_setFirstTimeIntoRXmodeAfterTXmode == 1)
            {
                    RX_PowerUp();
                    m_setFirstTimeIntoRXmodeAfterTXmode = 0;
            }

            if ( m_RX_flag )                  // if RX_flag == 1 then: - je�eli zmienna RX_flag == 1 to:
            {               CE_LOW;
                            uint8_t fifo_status; //variable which stores FIFO_STATUS register value
                            uint8_t len;         //variable which stores length of recived payload
                            uint8_t i;           //index variable for loop
                            uint8_t * wsk;       //pointer for first byte of reciver buffer nRF_RX_buffer
                            m_RX_flag = 0;         //reset the RX_flag

                            //do...while loop. this loop executes until RX_FIFO is empty
                            do
                            {
                            		memset(m_RX_bufffer, 0, sizeof(m_RX_bufffer));
                                    wsk = m_RX_bufffer;                                   //wsk equals to addres of first byte nRF_RX_bufffer
                                    CSN_LOW;                                                //chip select low
                                    m_spi->writeByte(R_RX_PL_WID);                             //send: read length of payload command
                                    len = m_spi->writeReadByte(NOP);                           //read length
                                    CSN_HIGH;                                               //chip select high
                                    if ( len > MAXIMUM_PAYLOAD_SIZE ) break;                                //if len is bigger than maximum payload size then break
                                    CSN_LOW;                                                //chip select low
                                    m_spi->writeByte( R_RX_PAYLOAD ); //read payload command
                                    i = len;
                                    while (i--)
                                    {
                                            *wsk++ = m_spi->writeReadByte(NOP);
                                    }
                                    CSN_HIGH;                    //csn goes high - csn stan wysoki
                                    CSN_LOW;
                                    uint8_t status = m_spi->writeReadByte(NOP);
                                    CSN_HIGH;
                                    status |= (1<<RX_DR);   //reset intterupt bit on status variable
                                    configRegister(STATUS, status);                    //save status variable in STATUS register       //than 0 send recived data to your callback function
                                    fifo_status = readOneByteFromRegister(FIFO_STATUS); //read FIFO_STATUS register
                                    if ( len &&  nRF_RX_Event_Callback ) ( * nRF_RX_Event_Callback )( m_RX_bufffer, len ); //if callback function is registered and len is bigger  //than 0 send recived data to your callback function
                            }
                            while ( !( fifo_status & (1<<RX_EMPTY) ));  //if RX_EMPTY bit is LOW then loop must execute.
    //                      CSN_LOW;
    //                      uint8_t status = SPI_WriteReadByte(NOP);
    //                      CSN_HIGH;
    //                      status |= (1<<RX_DR);                                                   //reset intterupt bit on status variable
    //                      nRF_Config_Register(STATUS, status);                    //save status variable in STATUS register       //than 0 send recived data to your callback function
                            CE_HIGH;
            }
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function which is used to register your own callback function
////........................................................................................................................
void CNrf::registerRX_Event_Callback ( void ( * callback )( void * nRF_RX_buff , uint8_t len ) )
{
	nRF_RX_Event_Callback = callback; //in this line we give an addres to our callback function
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to send data to another nRF's
//// attribute to this function is the pointer to the first byte of transsmision buffer
////........................................................................................................................
void CNrf::sendDataToAir( char * data )
{
	if(m_TX_flag == 1) return;								//if still is in transmitting mode or function isn't used for the first time, return
	m_TX_flag = 1;											//set TX mode flag to one

	char * wsk = data;										//wsk pointer is equal to the addres of first byte transmission buffer
	uint8_t length;											//declare length variable
	if(m_disableDynamicPayload == 1) length = m_payloadWidth;//if nRF_Set_PAYLOAD_Width() was used use the length variable is equal to width parameter of tha function
	else length = strlen ( data);						//else length is equal to the size of transmission buffer
	if ( length >= MAXIMUM_PAYLOAD_SIZE ) length = 31;
	TX_PowerUp();										//turn on transmission mode
	_delay_us(150);
	CSN_LOW;												//make CSN low
	m_spi->writeByte(W_TX_PAYLOAD);							//send to nRF W_TX_PAYLOAD command what means "Write data to TX FIFO"
	for (uint8_t i = 0; i < length; i++ )					//until "i" is smaller than "length" send bytes to nRF
	{
		m_spi->writeByte(*wsk++);
	}
	m_spi->writeByte(0);
	CSN_HIGH;												//CSN high
	CE_HIGH;												//start transmision!
	#if USE_IRQ == 0

	uint8_t data1 = 0;

	do
	{
		CSN_LOW;											//make CSN low
		data1 = m_spi->writeReadByte(NOP);						//send to nRF dummy byte and read STATUS register
		CSN_HIGH;											//make CSN high
	}while(!(( data1 & (1<<TX_DS))||( data1 & (1<<MAX_RT))));

	PORTA &= ~(1<<PA0);
	PORTA &= ~(1<<PA0);

	if ( ( data1 & (1<<MAX_RT) ) )							//if maximum ammount of retransmissions is achived,
	{
		CE_LOW;													//make CE low												//clear TX FIFO
		configRegister(STATUS, data1 | (1<<MAX_RT) | (1<<TX_DS) );	//reset TX_DS and MAX_RT intterrupt flags in STATUS register
		clear_TX();
		m_setFirstTimeIntoRXmodeAfterTXmode = 1;
		m_TX_flag = 0;
	}
															//else if was send or funcion is used for the first time
	if ( ( data1 & (1<<TX_DS)) )
	{
		CE_LOW;
		m_setFirstTimeIntoRXmodeAfterTXmode = 1;
		m_TX_flag = 0;										//TX flag = 0
		configRegister(STATUS, data1 | (1<<TX_DS));		//clear TX_DS int flag
	}

	#endif
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to chcecking RX_DR bit in STATUS register. When this bit is HIGH that means tha data are ready
//// function used in pooling mode
////........................................................................................................................
uint8_t CNrf::dataReady(void)
{
	CSN_LOW;									//make CSN low
	uint8_t data = m_spi->writeReadByte(NOP);		//send to nRF dummy byte and read STATUS register
	CSN_HIGH;									//make CSN high
	return (data & (1<<RX_DR));					//return 0 if data not ready or 1 if data ready
}

//void CNrf::initializeInterrupt(void)
//{
//
//}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to write data to the choosen register
////........................................................................................................................
void CNrf::configRegister(uint8_t register_name, uint8_t value)
{
	CSN_LOW;														//make CSN low
	m_spi->writeByte( W_REGISTER | (REGISTER_MASK & register_name) );  //send information to nRF which register you want to write
	m_spi->writeByte( value );                                         //send value that you want write to this register
	CSN_HIGH;
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set transmitter addres.
////........................................................................................................................
void CNrf::setTransmitterAdres(const char * addres)
{
	char TX_addres[TX_ADDRES_LENGTH];														//declaration of TX_addres temporary buffer

	for(uint8_t i = 0; i < TX_ADDRES_LENGTH; i++ )                                          //until i is smaller than TX_ADDRES LENGTH copy data from flash to temporary TX_addres buffer
	{
		//TX_addres[i] = pgm_read_byte(addres++);
		TX_addres[i] = *(addres++);
	}

	if( TX_ADDRES_LENGTH > 5 ) writeRegister(TX_ADDR, (uint8_t *) TX_addres, 5);		//if TX_ADDRES_LENGTH is bigger than 5 bytes send only 5 bytes
	else if( TX_ADDRES_LENGTH < 3 ) writeRegister(TX_ADDR, (uint8_t *) TX_addres, 3);  //if TX_ADDRES_LENGTH is smaller than 3 bytes send 3 bytes
	else writeRegister(TX_ADDR, (uint8_t *) TX_addres , TX_ADDRES_LENGTH);             //else send given by attribute ammount of bytes

}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set transmitter addres.
////........................................................................................................................
void CNrf::setReciverAddres( uint8_t data_pipe, const char * addres )
{
	char RX_addres[RX_ADDRES_LENGTH];														//declaration of RX_addres temporary buffer

	if( data_pipe > RX_ADDR_P5 ) data_pipe = RX_ADDR_P0; 								    //this is a protection. When given by data_pipe variable register addres is smaller
	else if( data_pipe < RX_ADDR_P0 ) data_pipe = RX_ADDR_P0;								// or bigger than possible value, this condition is setting deffault value
	else if( data_pipe > RX_ADDR_P5 ) data_pipe = RX_ADDR_P5;

	if ( data_pipe < RX_ADDR_P2 )															// data_pipe ix equal to RX_ADDR_P0 or RX_ADDR_P1 write 5 bytes of addres to nRF
	{
		for(uint8_t i = 0; i < RX_ADDRES_LENGTH; i++ )
		{
			//RX_addres[i] = pgm_read_byte(addres++);
			RX_addres[i] = *(addres++);
		}

		if( RX_ADDRES_LENGTH > 5 ) writeRegister(data_pipe, (uint8_t*)RX_addres, 5);
		else if( RX_ADDRES_LENGTH < 3 ) writeRegister(data_pipe, (uint8_t*) RX_addres, 3);
		else writeRegister(data_pipe, (uint8_t *) RX_addres, RX_ADDRES_LENGTH);
	}
	else																					//else send only one byte of addres because first four bytes are the same as the RX_ADDR_P1 first four bytes of addres.
	{
		//RX_addres[0] = pgm_read_byte(addres);
		RX_addres[0] = *(addres++);
		configRegister(data_pipe, RX_addres[0]);
	}
}

void CNrf::setReciverAddres( uint8_t data_pipe, uint16_t address)
{
	char buff[6];
	intToAddressArray(address, buff);
	setReciverAddres(data_pipe, buff);

}

void CNrf::setTransmitterAdres(uint16_t address)
{
	char buff[6];
	intToAddressArray(address, buff);
	setTransmitterAdres(buff);
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to read one byte from register given as attribute of the function
////........................................................................................................................
uint8_t CNrf::readOneByteFromRegister(uint8_t register_name)
{
	uint8_t data;       											//temporary variable data is used to store and return value of choosen register
	CSN_LOW;                                                        //chip select low
	m_spi->writeByte( R_REGISTER | (REGISTER_MASK & register_name) );  //send R_REGISTER command with number of choosen register (SEE nRF24L01_memory_map.h)
	data = m_spi->writeReadByte(NOP);                                  //write to "data", value from choosen register
	CSN_HIGH;                                                       //chip select high
	return data;
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to number of retransmissions and time beetwen each retransmission
//// first attribute is WAIT_XXXX_uS and the second is RETR_X_TIMES (see nRF_memory_map.h)
////........................................................................................................................
void CNrf::setRetransmissionTimeAndAmmount (uint8_t time, uint8_t ammount)
{
	configRegister( SETUP_RETR, ( time | ammount ) );		//write to SETUP_RETR register number of retransmisions and time beetwen them
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to switch nRF into TX mode
////........................................................................................................................
void CNrf::TX_PowerUp(void)
{
	CE_LOW;																	//make CE low
	uint8_t config;															//declaration of temporary variable
	config = readOneByteFromRegister(CONFIG);						//save copy of config regster in config variable
	config &= ~(1<<PRIM_RX);												//reset PRIM_RX bit
	config |= (1<<PWR_UP);													//set PWR_UP bit
	configRegister( CONFIG , config);									//write config variable to CONGIG register
					//make CE high
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to switch nRF into RX mode
////........................................................................................................................
void CNrf::RX_PowerUp(void)
{
	CE_LOW;																	//make CE_LOW
	uint8_t config;															//declaration of config variable
	config = readOneByteFromRegister(CONFIG);						//save copy of CONFIG register to config variable
	configRegister(CONFIG , config | (1<<PWR_UP) | (1<<PRIM_RX) );		//set PWR_UP and PRIM_RX bits in config variable ande save it in nRF CONFIG register
	CE_HIGH; //make CE_HIGH
	_delay_us(130);
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to switch nRF into POWER DOWN mode
////........................................................................................................................
void CNrf::powerDown(void)
{
	CE_LOW;																	//make CE_LOW
	uint8_t config;															//declaration of config variable
	config = readOneByteFromRegister(CONFIG);						//save copy of CONFIG register to config variable
	config &= ~((1<<PWR_UP) | (1<<PRIM_RX));								//reset PWR_UP and PRIM_RX bits
	configRegister(CONFIG , config );									//save congig variable in CONFIG register
	clear_RX();															//clear RX fifo
	clear_TX();															//clear TX fifo
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set transmission channel
////........................................................................................................................
void CNrf::setChannel( uint8_t channel )
{
	configRegister( RF_CH ,  0x7F & channel );							//save to nRF RF_CH register channel number (0x7f == 0b01111111 is the mask)
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to clear TX fifo
////........................................................................................................................
void CNrf::clear_TX (void)
{
	CSN_LOW;							//make CSN low
	m_spi->writeByte( FLUSH_TX );			//send to nRF FLUSH_TX command what means CLEAR TX FIFO
	CSN_HIGH;							//make CSN high
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to clear RX fifo
////........................................................................................................................
void CNrf::clear_RX (void)
{
	CSN_LOW;							//make CSN low
	m_spi->writeByte( FLUSH_RX );			//send to nRF FLUSH_RX command what means CLEAR RX FIFO
	CSN_HIGH;							//make CSN high
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to read data from the choosen register to buffer
////........................................................................................................................
void CNrf::readRegisters( uint8_t register_name, uint8_t * buffer_for_registers_content, uint8_t length_of_content )
{
	CSN_LOW;																								 	//make CSN low
	m_spi->writeByte( R_REGISTER | ( REGISTER_MASK & register_name ) );										 	//send information to nRF which register you want to read
	m_spi->writeReadDataBuffer( buffer_for_registers_content, buffer_for_registers_content, length_of_content );	//read from nRF given ammount of bytes to the buffer
	CSN_HIGH;       																						 	//make CSN high
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to write data from the buffer to the choosen register
////........................................................................................................................
void CNrf::writeRegister( uint8_t register_name, uint8_t * buffer_for_registers_content, uint8_t length_of_content )
{
	CSN_LOW;																									//make CSN low
	m_spi->writeByte( W_REGISTER | ( REGISTER_MASK & register_name ) );											//send information to nRF which register you want to write
	m_spi->writeReadDataBuffer( buffer_for_registers_content, buffer_for_registers_content, length_of_content );	//write to nRF given ammount of bytes from the buffer
	CSN_HIGH;																									//make CSN low
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set states of datapipes
//// first attribute is DataPipe, you must put here name of datapipe tha you want to enable, this name is ERX_Px where x is number of data pipe (see nRF_Init function)
//// second attribute is or_on_off, here you put ON or OFF macro. When you put here ON, the choosen data pipe will be turned ON, if put here OFF, datapipe will be turned off
//// third and last parameter is ACK_on_or_off, you must put here macros: ACK_ON or ACK_OFF. This parameter turns ACK_ON or turns ACK_OFF
////........................................................................................................................
void CNrf::setActiveDataPipeAndACK (uint8_t DataPipe, uint8_t on_or_off, uint8_t ACK_on_or_off )
{
	uint8_t data = readOneByteFromRegister(EN_RXADDR);		//read data from EN_RXADDR register save it in data variable

	if(on_or_off == ON) data |= (DataPipe);							//if second attribute is ON, set bit ERX_Px (given by DataPipe parameter)
	else if (on_or_off == OFF) data &= ~(DataPipe);					//else reset this bit

	configRegister( EN_RXADDR, data );							//save data variable to EN_RXADDR register

	data = readOneByteFromRegister(EN__AA);					//read data from EN_AA register and save it in data variable

	if(ACK_on_or_off == ACK_ON) data |= (DataPipe);					//if third parameter is ACK_ON, enable auto ackonwledgements by setting ENAA_Px bit (this same value as ERX_Px)
	else if (ACK_on_or_off == ACK_OFF) data &= ~(DataPipe);			//else turn off auto acknowledgments

	configRegister( EN__AA, data );							//save the variable data to EN__AA register

}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set payload width
//// first attribute of this function is number or RX payload (RX_PW_Px), second one is the width(1-32 bytes)
//// use this function only if the dyanmic payloads are turned off
////........................................................................................................................
void CNrf::setPayloadWidth ( uint8_t payload, uint8_t width )
{
	configRegister( payload, ( 0x1F & width ) );		//config RX_PW_Px with number of bytes
	m_disableDynamicPayload = 1;							//set the disable_dynamic_payload flag
	m_payloadWidth = width;									//global variable payload_width is equal to width given by attribute
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set dynamic payloads
//// first attribute is DPL_Px (number of receiver payload) and the second is ON or OFF(see nRF_memory_map.h)
////........................................................................................................................
void CNrf::setDynamicPayloadStateOnDataPipe(uint8_t data_pipe_number, uint8_t on_off )
{
	configRegister( DYNPD, data_pipe_number );					//save DPL_Px value to DYNPD register
	uint8_t feature = readOneByteFromRegister(FEATURE);		//make copy of FEATURE register

	if (on_off == ON)	feature |= (1<<EN_DPL);		//if second attribute is equal to ON
														//set bit EN_DPL
	if (on_off == OFF)	feature &= ~(1<<EN_DPL);	//if second attribute is equal to OFF
														//reset bit EN_DPL
	configRegister(FEATURE, feature);			//save feature variable to FEATURE register
}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set transmission speed and receiver power
//// first attribute is TRANS_SPEED_x and the second is RF_PWR_x (see nRF_memory_map.h)
////........................................................................................................................
void CNrf::setDataSpeedAndReciverPower(uint8_t Data_rate, uint8_t power)
{
	configRegister( RF_SETUP, ( Data_rate | power ));	 	//write to RF_SETUP register transmision speed and and receiver power

}

////''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//// function used to set state and width of CRC. First attrbiute of this function is ONE_BYTE or TWO_BYTES,
//// second attribute is ON or OFF
////........................................................................................................................
void CNrf::setStateAndWidthOfCRC( uint8_t one_or_two_bytes , uint8_t on_or_off)
{
	uint8_t config = readOneByteFromRegister(CONFIG);				//read data from congig register and save it to config variable

	if ( one_or_two_bytes == ONE_BYTE ) config &= ~(1<<CRCO);				//if first given attribute is ONE_BYTE clear CRCO bit
	else if ( one_or_two_bytes == TWO_BYTES ) config |= (1<<CRCO);			//else if first given attribute is TWO_BYTES set CRCO bit
	else config &= ~(1<<CRCO);												//else given attribute is diffrent set CRCO low

	if (on_or_off == ON)   config |= (1<<EN_CRC);							//if second attribute is equal to ON, set EN_CRC bit
	else if ( on_or_off == OFF )  config &= ~(1<<EN_CRC);					//esle reset the EN_CRC bit

	configRegister(CONFIG, config);									//write config content to CONFIG register
}

#if USE_IRQ == 1
//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''//
//         IRQ INTERRUPT HANDLER section					                   //
//.............................................................................//

//in this function you must put your initialization of external intterup (INTx or PCINTx)
//if you don't use interrupts there is no matter what code is here

void CNrf::initializeInterrupt(void)
{
#if (defined(__AVR_ATmega8__))
	GICR |= (1 << INT0);      // Turns on INT0
	MCUCR |= (1 << ISC01);
#elif ( defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__) )
	GICR   |= (1<<INT2);              //enable INT2 intterupt source in GICR register
	MCUCSR |= (0<<ISC2);              //set intterupt active on falling edge
#elif ( defined(__AVR_ATmega328P__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) )
	EIMSK   |= (1<<INT0);              //enable INT2 intterupt source in GICR register
	EICRA |= (1<<ISC01);              //set intterupt active on falling edge

#endif

}

//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// Here is external intterupt handler, if you are using interupts change the name of the vector interrupt ore live the same if you are using the same
//...................................................................................................................................................
void INT0_vect(void)
{
	CSN_LOW;											//Chip select LOW
	register uint8_t status = CNrf::getInstance()->m_spi->writeReadByte(NOP);	//read STATUS register value
	CSN_HIGH;											//chip select high

	if ( (status & (1<<RX_DR)) )						//id data recived
	{
		CNrf::getInstance()->m_RX_flag = 1;									//RX flag equals to 1
	}

	if ( status & (1<<TX_DS) )							//if data was send
	{

		CNrf::getInstance()->m_setFirstTimeIntoRXmodeAfterTXmode = 1;
		status |= (1<<TX_DS);							//clear TX_DS bit in status variable
		CNrf::getInstance()->configRegister( STATUS, status );			//save status variable in STATUS register
		CNrf::getInstance()->m_TX_flag = 0;									//TX_flag = 0
		CE_LOW;
//		nRF_RX_Power_Up();//enable receiving mode
	}

	if ( status & (1<<MAX_RT) )							//id max of retransmissions was achived
	{
		status |= (1<<MAX_RT) | (1<<TX_DS);				//clear MAX_RT and TX_DS bits in status variable
		CNrf::getInstance()->m_TX_flag = 0;									//clear TX_flag
		CNrf::getInstance()->configRegister( STATUS, status );			//save variable status to the STATUS register
		CNrf::getInstance()->m_setFirstTimeIntoRXmodeAfterTXmode = 1;
		CNrf::getInstance()->clear_TX();
		CE_LOW;
	}
}

#endif

