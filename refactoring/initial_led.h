/********************************************
* LED CONTROL Initial
* PORTD
*********************************************/
#define DDRD_LED		DDRD
#define PORTD_LED		PORTD

void Init_LED_CTRL()
{
	DDRD_LED = 0xFF;
	PORTD_LED = 0x00;
}
 
