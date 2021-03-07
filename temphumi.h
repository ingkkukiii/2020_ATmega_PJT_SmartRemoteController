#define    DATAOUT     		DDRF|=1
#define    SHTOUT     		DDRF|=2

#define    DATA_OUT_1  		PORTF|=1
#define    DATA_OUT_0  		PORTF&=~1

#define    SHT_SCK_1      	PORTF|=2
#define    SHT_SCK_0      	PORTF&=~2

#define    DATAIN     		DDRF&=~1
#define    DATA_IN        	PINF&1

#define 	noACK 	0
#define 	ACK 	1

#define 	STATUS_REG_W 	0x06 //000 0011 0
#define 	STATUS_REG_R 	0x07 //000 0011 1
#define 	MEASURE_TEMP 	0x03 //000 0001 1
#define 	MEASURE_HUMI 	0x05 //000 0010 1
#define 	RESET 			0x1e //000 1111 0

char str[50];
char str2[60];

float dew_point;
enum {TEMP,HUMI};

unsigned char error;
unsigned char checksum;

typedef union
{ 
	int i;
	float f;
} value;

char s_write_byte(unsigned  char  value)
{
	unsigned char  i, error=0;

	DATAOUT; 
	SHTOUT;

	for (i=0x80; i>0; i>>=1)
	{ 	if (i & value)
			DATA_OUT_1; 
		else 
			DATA_OUT_0;
		_delay_us(3); 

		SHT_SCK_1; 
		_delay_us(6); 

		SHT_SCK_0;
		_delay_us(3); 
	 }

	DATA_OUT_0; 
	_delay_us(3); 
	SHT_SCK_1; 
	_delay_us(3);

	DATAIN;
	_delay_us(3);

	if(DATA_IN) 
		error=1; 
	else 
		error=0;  

	SHT_SCK_0;
	_delay_ms(400);

	return error; 
}

char s_read_byte(unsigned  char  ack)
{
	unsigned char  i, val = 0;
	DATAOUT; 
	SHTOUT;
	_delay_us(3);
	
	DATA_OUT_1;
	_delay_us(3);
	
	DATAIN;
	
	for (i=0x80; i>0; i>>=1)
	{ 	
		SHT_SCK_1; 
	    	_delay_us(3);

		if (DATA_IN) val = (val | i); 
		
		SHT_SCK_0;
		_delay_us(3);
	}
	DATAOUT;
	_delay_us(3); 
	
	if(ack) 
		DATA_OUT_0; 
	else 
		DATA_OUT_1;
	_delay_us(3); 

	SHT_SCK_1; 
	_delay_us(6); 

	SHT_SCK_0;
	_delay_us(3); 

	DATA_OUT_1; 
	return val;
} 

void s_transstart(void)
{
  	DATAOUT; 
	SHTOUT;

	DATA_OUT_1; 
	SHT_SCK_0; 
	_delay_us(3);
	SHT_SCK_1;
	_delay_us(3);
	DATA_OUT_0;
	_delay_us(3);
	SHT_SCK_0;
	_delay_us(6);
	SHT_SCK_1;
	_delay_us(3);
	DATA_OUT_1;
	_delay_us(3);
	SHT_SCK_0;
}

void  s_connectionreset(void)
{
	unsigned char i;

	DATAOUT; 
	SHTOUT;
	DATA_OUT_1; 
	SHT_SCK_0; 

	for(i=0; i<9; i++) 	
	{ 
		SHT_SCK_1;
		_delay_us(3);
		SHT_SCK_0;
		_delay_us(3);
	}
	s_transstart(); 	
}

char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char  mode)
{
	unsigned char error = 0;
	unsigned int i;
	s_transstart(); 
	switch(mode)
	{
		case TEMP : 
			error += s_write_byte(MEASURE_TEMP); 
			break;                  // 000 00011
		case HUMI : 
			error += s_write_byte(MEASURE_HUMI); 
			break; 				   // 000 00101
		default : 
			break;
	}
    	DATAIN;
	_delay_us(3);

	for (i=0;i<65535;i++) 	
		if(!DATA_IN) break; 
	if(DATA_IN) 
		error += 1; 

	*(p_value+1) = s_read_byte(ACK);

	*(p_value) = s_read_byte(ACK); 

	*p_checksum = s_read_byte(noACK); 

	return error;
}

void calc_sth11(float  *p_humidity, float  *p_temperature) 
{
	const float C1 = -4.0; 
	const float C2 = +0.0405;
	const float C3 = -0.0000028;
	const float T1 = +0.01; 
	const float T2 = +0.00008; 
	float rh = *p_humidity; 	
	float t = *p_temperature; 
	float  rh_lin; 
	float  rh_true;
	float  t_C; 

	t_C = t*0.01 - 40.0;
	rh_lin = C3*rh*rh + C2*rh + C1;
	rh_true = (t_C-25)*(T1+T2*rh) + rh_lin; 
	if(rh_true > 100)	rh_true = 100; 
	if(rh_true < 0.1)	rh_true = 0.1; 
	*p_temperature = t_C; 
	*p_humidity = rh_true;
}

float calc_dewpoint(float h, float t)
{ 
	float dew_point, temp_d;

	if(t >= 0)
	{
		temp_d = (17.62 * t)/(243.12 + t);
		dew_point = 243.12 * (log(h/100) + temp_d) / (17.62 - log(h/100) - temp_d);
	}
	else
	{
		temp_d = (22.46 * t)/(272.62 + t);
		dew_point = 272.62 * (log(h/100) + temp_d) / (22.46 - log(h/100) - temp_d);
	}

	return dew_point;
}

//STH Sensor Config//
void Temp_sensor()
{
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
}

