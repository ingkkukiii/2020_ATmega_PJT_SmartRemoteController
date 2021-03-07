#include "2020_Project_SmartAirCon.h" //1105 점검
 
volatile unsigned int tic_time;    

unsigned int i;
unsigned char data;
unsigned char flag = 9;
unsigned char k = 0;

value humi_val, temp_val;
extern unsigned char error;
extern unsigned char checksum;

char str[50];

int speed = 255; //OCR0 = 255 MOTOR STOP

void ledpower(void)
{
		if(OCR0 == 0)
		{
			for(k=0;k<8;k++)	
			{
				PORTD = (1 << k);
				_delay_ms(50);
			}
		}
		else if(OCR0 == 30)
		{
			for(k=0;k<8;k++)	
			{
				PORTD = (1 << k);
				_delay_ms(70);
			}
		}
		else if(OCR0 == 60)
		{
			for(k=0;k<8;k++)	
			{
				PORTD = (1 << k);
				_delay_ms(100);
			}
		}
		return 0;
}
//USART0 Interrupt Config PART// 
ISR(USART0_RX_vect) //SIGNAL(SIG_USART0_RECV)
{
	cli();  

	data = UDR0;  //usart_recv_byte(); //UDRn : 전송/수신 데이터 버퍼 레지스터 

	switch(data)
	{
		//에어컨 Off
		case '1' :	
					speed=255;
					flag=1;  
					setPosition(1,0);
					waitBusy();
					writeSTR("YS Aircon LV OFF");
					data = 0;  //ASCII 숫자 0 => NULL
					break;
		//에어컨 LV.1
		case '2' :  
					speed=60;;
					flag=2;
					setPosition(1,0);
					waitBusy();
					writeSTR("YS Aircon LV1 ON");	
					break;
		//에어컨 LV.2
		case '3' :  
					speed=30;;
					flag=3;
					setPosition(1,0);
					waitBusy();
					writeSTR("YS Aircon LV2 ON");	
					data = 0;
					break;
		//에어컨 LV.3
		case '4' :  
					speed=0;;
					flag=4;
					setPosition(1,0);
					waitBusy();
					writeSTR("YS Aircon LV3 ON");	
					data = 0;
					break;
		default : break; //다른키 눌리면 아무것도 안함
	}
	sei(); //중요 //휴대폰->마이컴 동작
}

//KEY Interrupt Config PART// 
ISR(INT7_vect)//SIGNAL(SIG_INTERRUPT7) 
{	
	//KEY 7 : 에어컨 LV.3

	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV3 ON");	
	flag = 4;
	speed = 0; 

}
ISR(INT6_vect)//SIGNAL(SIG_INTERRUPT6)  
{	
	//KEY 6 : 에어컨 LV.2

	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV2 ON");	

	flag = 3;
	speed = 30; 

}
ISR(INT5_vect)//SIGNAL(SIG_INTERRUPT5)
{
	//KEY 5 : 에어컨 LV.1
	
	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV1 ON");	

	flag = 2;
	speed = 60; 
	
}
ISR(INT4_vect)//SIGNAL(SIG_INTERRUPT4)
{
	//KEY 4 : 에어컨 OFF
	
	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV OFF");

	flag = 1;
	speed = 255;	
}

//SysTick Config PART//
ISR(TIMER0_COMP_vect) 
{
	tic_time++;  
}
void delay_s(unsigned int dsec) 
{
	tic_time = 0;   
	while(dsec > tic_time); 
}

//Main Project//
int main()
{	
	//LED
	DDRD = 0xFF;
	PORTD = 0x00;

	//Motor PORTB//
	DDRB_DCMOTOR = 0xFF;
	PORTB_DCMOTOR = 0x00;

	//KEY Button PORTE//
	DDRE_BUTTON = 0x00; 

	//
	EIMSK = (1<<INT7) | (1<<INT6) | (1<<INT5) | (1<<INT4);
	EICRB = (1<<ISC71) | (1<<ISC70) | (1<<ISC61) | (1<<ISC60) | 
			(1<<ISC51) | (1<<ISC50) | (1<<ISC41) | (1<<ISC40);	
	TCCR0 = (1<<FOC0) | (0<<WGM01) | (1<<WGM00) | (1<<COM01) | (1<<COM00) | 
			(0<<CS02) | (0<<CS01) | (1<<CS00);
	TCNT0 = 0;
	OCR0 = 255;
	TIMSK = (1<<OCIE0) | (0<<TOIE0); //compare match

	//LCD PORTA, PORTC//
	portOut();
	LCDInit();
	waitBusy();

	//Temphumi Reset PORTF//
	s_connectionreset();

	//Bluetooth Init USART0//
	Init_USART();
	
	sei();

	while(1)
	{
		//STH Sensor Config//
		value humi_val, temp_val;

		error = 0;
		error += s_measure((unsigned char*) &humi_val.i, &checksum, HUMI);
		error += s_measure((unsigned char*) &temp_val.i, &checksum, TEMP);

		if(error != 0)
			s_connectionreset();
		else
		{
			humi_val.f = (float)humi_val.i;
			temp_val.f = (float)temp_val.i;
			calc_sth11(&humi_val.f, &temp_val.f);
			dew_point = calc_dewpoint(humi_val.f, temp_val.f);

			setPosition(0,0);
			waitBusy();

			dtostrf(temp_val.f, 4, 1, str);
			writeSTR("TP:");
			waitBusy();
			writeSTR(str);
			waitBusy();
	
			TX0_STR(str);

			cursorShiftRight();
			waitBusy();

			dtostrf(humi_val.f, 4, 1, str);
			writeSTR("HM:");
			waitBusy();
			writeSTR(str);
			waitBusy();
		}

		ledpower();

		//DC Motor Config//
		if(temp_val.f >= 24.0)
		{
			PORTB_DCMOTOR = 0x30;
			OCR0 = 60;
			delay_s(1);
				
			if(flag==1) //에어컨 Off
	 		{
				for(;;)
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				if(temp_val.f < 24.0) flag = 9;
			}
	 		else if(flag==2)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				if(temp_val.f < 24.0) flag = 9;
			}
			else if(flag==3)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				if(temp_val.f < 24.0) flag = 9;
			}
			else if(flag==4)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				if(temp_val.f < 24.0) flag = 9;
			}
	 	}
		else
		{
			PORTB_DCMOTOR = 0x70;
			OCR0 = 255;
			delay_s(1);
			PORTD = 0x00;

			if(flag==1)	//에어컨 Off
	 		{
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				flag=9;
			}
	 		else if(flag==2)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
				
			}
			else if(flag==3)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
		
			}
			else if(flag==4)
	 		{	
				PORTB_DCMOTOR = 0x30;
				OCR0 = speed;
				delay_s(1);
			}
		}
	}
	return 0;
}
