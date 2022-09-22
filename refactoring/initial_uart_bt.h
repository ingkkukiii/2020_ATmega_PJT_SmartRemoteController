/********************************************
* UART Serial Initial
* Serial Intercae
* Full-duplex transmission
* HC06 Serial-Blooth Intercae Module  
*********************************************/
#define CPU_CLOCK 16000000UL

#define BAUD_RATE 9600
#define BAUD_RATE_L (CPU_CLOCK / (16L * BAUD_RATE)) - 1
#define BAUD_RATE_H ((CPU_CLOCK / (16L * BAUD_RATE)) - 1) >> 8

void Init_UART()
{
	UBRR0L = (unsigned char)BAUD_RATE_L;
	UBRR0H = (unsigned char)BAUD_RATE_H;

	UCSR0B = (1 << RXCIE0) | (1 << TXEN0) | (1 << RXEN0) | (0 << UCSZ02);
	UCSR0C = (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
}

//문자 전송 보내기 
void usart_send_byte(unsigned char byte)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
}

//문자열 전송 보내기
void usart_send_string(unsigned char *str, unsigned char len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		if (!(*(str + i)))
		{
			break;
		}
		usart_send_byte(*(str + i));
	}
}

//문자 전송 받기
unsigned char usart_recv_byte(void)
{
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

//
void TX0_CHAR(char ch)
{ 
	while(!(UCSR0A&0x20)); UDR0=ch; 
}
void TX0_STR(char *s)
{ 
	while(*s)TX0_CHAR(*s++); 
}
//

//interrupt [USART0_RXC] 
void usart0_rx_isr(void) //수신 인터럽트
{ 
  char ch;
  ch=UDR0;
}
