/*
 * uart.h
 *
 *  Created on: 10.03.2017
 *      Author: Damian
 */

#ifndef UART_H_
#define UART_H_

#include <string.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#define UART_BAUD 500000		// tu definiujemy interesuj�c� nas pr�dko��
#define __UBRR ((F_CPU+UART_BAUD*8UL) / (16UL*UART_BAUD)-1) // obliczamy UBRR dla U2X=0

//// definicje na potrzeby RS485
//#define UART_DE_PORT PORTD
//#define UART_DE_DIR DDRD
//#define UART_DE_BIT (1<<PD2)
//
//#define UART_DE_ODBIERANIE  UART_DE_PORT |= UART_DE_BIT
//#define UART_DE_NADAWANIE  UART_DE_PORT &= ~UART_DE_BIT

#define UART_RX_BUF_SIZE 64 // definiujemy bufor o rozmiarze 32 bajt�w
// definiujemy mask� dla naszego bufora
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

#define UART_TX_BUF_SIZE 62 // definiujemy bufor o rozmiarze 16 bajt�w
// definiujemy mask� dla naszego bufora
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)


extern "C"
{
	void USART_UDRE_vect (void) __attribute__ ((signal,used, externally_visible)) ;
	void USART_RXC_vect (void) __attribute__ ((signal,used, externally_visible)) ;
}

class CUart
{
public:
	//Public interface
	static CUart * getInstance();
	static void setBaudRate(uint32_t baud);
	int getc(void);
	void putc(const char data );
	void puts(const char *s);
	void putint(int value, int radix);
	void putll(long long int value, int radix);
	void puts_P(const char *s);
	char * getStr(char * buf);
	void RX_STR_EVENT(/*char * rbuf*/);
	void registerEventCallback(void (*callback)(char * pBuf));

	friend void USART_UDRE_vect (void);
	friend void USART_RXC_vect (void);

private:
	CUart(uint16_t baud);
	CUart(const CUart&);	//Prevent construction by copying
	CUart & operator=(const CUart&);	//Prevent assigment
	~CUart();					//Prevent unwanted destruction

	static CUart * m_cuUartInstance;
	static bool m_bIsCreated;

	volatile char m_cRxBuff[UART_RX_BUF_SIZE];
	volatile uint8_t m_u8_RxHead;
	volatile uint8_t m_u8_RxTail;

	volatile char m_cTxBuff[UART_TX_BUF_SIZE];
	volatile uint8_t m_u8_TxHead;
	volatile uint8_t m_u8_TxTail;

	volatile uint8_t m_u8_asciiLine;
	void (*uart_rx_str_event_callback)(char * pBuf);

	static uint16_t m_sBaudRate;
	char m_uartBuffer[100] = {};
};

#endif /* UART_H_ */
