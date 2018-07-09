/*
 * uart.cpp
 *
 *  Created on: 10.03.2017
 *      Author: Damian
 */


#include "uart.h"

//CTerminal * terminal = reinterpret_cast<CTerminal *> (uart);

bool CUart::m_bIsCreated = 0;
CUart * CUart::m_cuUartInstance = nullptr;
uint16_t CUart::m_sBaudRate = 9600;

CUart * CUart::getInstance()
{
	if(!m_bIsCreated)
	{
		static CUart uart(m_sBaudRate);
		m_cuUartInstance = &uart;
		m_bIsCreated = 1;
	}
	return m_cuUartInstance;
}

void CUart::setBaudRate(uint32_t baud)
{
	m_sBaudRate = baud;
}

CUart::CUart(uint16_t baud)
{
	/* Ustawienie pr�dko�ci*/

	UBRRH = (uint8_t)(baud>>8);
	UBRRL = (uint8_t)baud;

	/* Za��czenie nadajnika I odbiornika*/
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Ustawienie format ramki: 8bit�w danych, 1 bit stopu */
	UCSRC = (1<<URSEL)|(3<<UCSZ0);

	// je�li korzystamy z interefejsu RS485
	#ifdef UART_DE_PORT
		// inicjalizujemy lini� steruj�c� nadajnikiem
		UART_DE_DIR |= UART_DE_BIT;
		UART_DE_ODBIERANIE;
	#endif

	// je�li korzystamy z interefejsu RS485
	#ifdef UART_DE_PORT
		// je�li korzystamy z interefejsu RS485 za��czamy dodatkowe przerwanie TXCIE
		UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	#else
		// jeżeli nie  korzystamy z interefejsu RS485
		UCSRB |= (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	#endif

	//Now time for zero member values
	memset((void*) m_cRxBuff, 0, sizeof(m_cRxBuff)); //clear buffer
	memset((void*) m_cTxBuff, 0, sizeof(m_cTxBuff)); //clear buffer
	m_u8_RxHead = 0;
	m_u8_RxTail = 0;
	m_u8_TxHead = 0;
	m_u8_TxTail = 0;
	m_u8_asciiLine = 0;
	uart_rx_str_event_callback = nullptr;
}

CUart::~CUart()
{

}

void CUart::registerEventCallback(void (*callback)(char * pBuf))
{
	uart_rx_str_event_callback = callback;
}

char * CUart::getStr(char * buf)
{
	int c;
	char *wsk = buf;
	if(m_u8_asciiLine)
	{

		while( (c = getc()) )
		{
			if(c == 13 || c < 0) break;
			*(buf++) = c;
		}
		*buf = 0;
		m_u8_asciiLine--;
	}
	return wsk;
}

void CUart::RX_STR_EVENT(/*char * rbuf*/)
{
	if(m_u8_asciiLine)
	{
		if(uart_rx_str_event_callback)
		{
			getStr(m_uartBuffer);
			(*uart_rx_str_event_callback)(m_uartBuffer);
		}
		else
		{
			m_u8_RxHead = m_u8_RxTail;
		}
	}
}

//Add one byte to buffor
void CUart::putc(const char data)
{
	uint8_t u8_tmpHead;
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
	{
		u8_tmpHead = (m_u8_TxHead + 1) & UART_TX_BUF_MASK;

	}

	//This loop is waining if buffor is full
	while(u8_tmpHead == m_u8_TxTail){}


	m_cTxBuff[u8_tmpHead] = data;
	m_u8_TxHead = u8_tmpHead;


	//Start sending interrupt - now interrupt takes care about sending
	UCSRB |= (1<<UDRIE);

}

//Send string
void CUart::puts(const char *s)
{
	register char c;
	while( (c = *s++)) putc(c);

}

void CUart::puts_P(const char *s)		// wysy�a �a�cuch z pami�ci RAM na UART
{
  register char c;
  while ((c = pgm_read_byte(s++))) putc(c);			// dop�ki nie napotkasz 0 wysy�aj znak
}

void CUart::putint(int value, int radix)
{
	char string[17];
	itoa(value, string, radix);
	puts(string);
}

int CUart::getc(void)
{
	int nData = -1;
	//Check if indexes are equals
	if(m_u8_RxHead == m_u8_RxTail) return nData;
	ATOMIC_BLOCK( ATOMIC_RESTORESTATE )
	{
		//Calculate and save new tail index
		m_u8_RxTail = (m_u8_RxTail + 1) & UART_RX_BUF_MASK;
		//Return data from buffer
		nData = m_cRxBuff[m_u8_RxTail];
	}
	return nData;
}

//Interrupt for sending data. It takes data from buffer
void USART_UDRE_vect (void)
{
	//Check if indexes are different
	if(CUart::m_cuUartInstance->m_u8_TxHead != CUart::m_cuUartInstance->m_u8_TxTail)
	{
		//Calculate new index of tail
		CUart::m_cuUartInstance->m_u8_TxTail = (CUart::m_cuUartInstance->m_u8_TxTail + 1) & UART_TX_BUF_MASK;
#ifdef UART_DE_PORT
    	UART_DE_NADAWANIE;
#endif
    	UDR = CUart::m_cuUartInstance->m_cTxBuff[CUart::m_cuUartInstance->m_u8_TxTail];
	}
	else
	{
		//Nothing in buffer - clear interrupt flag
		UCSRB &= ~(1<<UDRIE);

	}
}

void USART_RXC_vect (void)
{
	register uint8_t u8_tmpHead;
	register char cData;

	//Take byte from hardware buffer
	cData = UDR;

	//Calculate new index of buffers head
	u8_tmpHead = (CUart::m_cuUartInstance->m_u8_RxHead + 1) & UART_RX_BUF_MASK;

	if( u8_tmpHead == CUart::m_cuUartInstance->m_u8_RxTail)
	{
		CUart::m_cuUartInstance->m_u8_RxHead = CUart::m_cuUartInstance->m_u8_RxTail;
	}
	else
	{

		switch(cData)
		{
		case 0:
		{

		}
		case 10:
		{
			break;		//ignore LF sign
		}
		case 13:
		{
			CUart::m_cuUartInstance->m_u8_asciiLine++;	//new line in buffer
		}

		default:
		{
			//CUart::m_cuUartInstance->datanumber++;
			CUart::m_cuUartInstance->m_u8_RxHead = u8_tmpHead;
			CUart::m_cuUartInstance->m_cRxBuff[u8_tmpHead] = cData;
		}
		}
	}
}





