/********************************************
* DC MOTOR Initial
* PWM(Pulse Width Modulation) mode - duty ratio
* Timer/Counter - CTC mode - PCPWM mode
* 16Mhz - 16M times per sec.
* TCCR0,2 Timer/Counter0,2 Control Reg. - FOC0,2(Bit7) defalut = 0, 1 > OCn pin Output 
* PORTB - PB4(0b00010000) - OC0 
*********************************************/
#define DDRB_DCMOTOR	DDRB
#define PORTB_DCMOTOR	PORTB

void Init_DCMotor_CTRL()
{
	DDRB_DCMOTOR = 0xFF;
	PORTB_DCMOTOR = 0x00;
}
 
