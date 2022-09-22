/********************************************
* INTERNAL INTERRUPT Initial
* Timer/Counter - Overflow, CTC mode
* 16Mhz - 16M times per sec.
*********************************************/
#define DDRE_BUTTON		DDRE
#define CPU_CLOCK		16000000UL

volatile unsigned int tic_time;   

/*
ISR(TIMER0_OVF_vect) //overflow match mode
{
	tic_time++;  
	TCNT0 = 256 - (CPU_CLOCK/64/1000); //256 - 250 = 6
}
*/

ISR(TIMER0_COMP_vect) //compare match mode
{
	tic_time++;  
}

void delay_s(unsigned int dsec) 
{
	tic_time = 0;   
	while(dsec > tic_time); 
}
 
//Timer/Counter Overflow mode Interrupt
void Init_TIMEINT_OVF()
{
		
	TCCR0 = (1<<CS02) | (1<<CS01) | (1<<CS00); //1024 clock prescaler

	TCNT0 = 256 - (CPU_CLOCK/64/1000); //256 - 250 = 6

	TIMSK = (0<<OCIE0) | (1<<TOIE0); //Overflow mode enable
	 
}

//Timer/Counter CTC(Clear Timer on Compare Match)mode Interrupt
void Init_TIMEINT_CTC()
{
		
	TCCR0 = (0<<WGM00) | (1<<WGM01) | (1<<CS02) | (0<<CS01) | (0<CS00); //1024 clock prescaler

	TIMSK = (1<<OCIE0) | (0<<TOIE0); //CompareMatch mode enable
	
	TCNT0 = 0;

	OCR0 = (CPU_CLOCK / 64 / 1000); //250
}


/********************************************
* EXTERNAL INTERRUPT Initial
* PORTE(INT4~INT7) - EICRB
*********************************************/

void Init_BUTTON()
{
	DDRE_BUTTON = 0x00; //0b0000xxxx
}


//External Interrupt KEY ISR
ISR(INT7_vect)//SIGNAL(SIG_INTERRUPT7) 
{	
	//KEY 7 : 에어컨 LV.3

	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV3 ON");	
}

ISR(INT6_vect)//SIGNAL(SIG_INTERRUPT6)  
{	
	//KEY 6 : 에어컨 LV.2

	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV2 ON");	
 
}

ISR(INT5_vect)//SIGNAL(SIG_INTERRUPT5)
{
	//KEY 5 : 에어컨 LV.1
	
	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV1 ON");	
 
}

ISR(INT4_vect)//SIGNAL(SIG_INTERRUPT4)
{
	//KEY 4 : 에어컨 OFF
	
	setPosition(1,0);
	waitBusy();
	writeSTR("YS Aircon LV OFF");
 	
}

//External Interrupt Initial
void Init_EXT_ISR()
{
	//External Interrupt Mask Reg.
	EIMSK = (1<<INT7) | (1<<INT6) | (1<<INT5) | (1<<INT4); 
	
	//External Interrupt Control trigger Reg. B 
	EICRB = (1<<ISC71) | (1<<ISC70) | (1<<ISC61) | (1<<ISC60) | 
			(1<<ISC51) | (1<<ISC50) | (1<<ISC41) | (1<<ISC40);	
}
