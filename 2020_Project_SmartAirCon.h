#include <avr/io.h>
#include <avr/delay.h> 
#include <avr/interrupt.h>

#include "LCD.h"
#include "temphumi.h"
#include "bluetooth.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define DDRB_DCMOTOR	DDRB
#define PORTB_DCMOTOR	PORTB

#define DDRE_BUTTON	DDRE

//#define DDRF_ADC	DDRF

//#define DDRG_LED	DDRG
//#define PORTG_LED	PORTG


