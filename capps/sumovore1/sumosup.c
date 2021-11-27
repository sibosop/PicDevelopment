#include <pic.h>
#include <sys.h>
#include "types.h"
#include "sumosup.h"


void
initSumo()
{
	TRISA  = 0xff;	// make port A all inputs
	TRISE  = 0x7;	// make port E all inputs
	PORTE  = 0;		// clear any digital stuff
	PORTA  = 0;		// clear any digital stuff.
	TRISB  = 0;		// make port b all outputs
	PORTB  = 0;
	TRISC  = 0;		// PORT C is all outputs
	PORTC  = 0;	
	TRISD = 0xE0;	// RD7 RD6 Ir inputs, the rest leds
	clearLeds();
	ADCON1 = 0;		// make port A all analog inputs
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	SUMO_STOP;
	initIr();
}

void 
clearLeds()
{
	
	PORTD &= ~0x1F;
	
}
void
setLed(UInt8 led, UInt8 on)
{
	if ( on )
		PORTD |= 1 << led;
	else
		PORTD &= ~(1 << led);
}

UInt8	irFlag;
UInt16	irTimer;

#define IR_DEBOUNCE_TIME	50
#define IR_ON_LEFT		1
#define IR_ON_RIGHT 	2	
#define IR_STATE_LEFT	4
#define IR_STATE_RIGHT	8
void
initIr()
{
	irFlag = 0;
	irTimer = 0;
}



		
void
checkIr()
{
	if ( ++irTimer >= IR_DEBOUNCE_TIME )
	{
		irTimer = 0;
		if ( IR_LEFT )
		{  
			if ( irFlag & IR_STATE_LEFT ) 
				irFlag |= IR_ON_LEFT; 
			else 
				irFlag |= IR_STATE_LEFT;
		} 
		else
		{ 
			if ( !(irFlag & IR_STATE_LEFT) )  
				irFlag &= ~IR_ON_LEFT; 
			else 
				irFlag &= ~IR_STATE_LEFT;
		} 
		if ( IR_RIGHT )
		{ 
			if ( irFlag & IR_STATE_RIGHT ) 
				irFlag |= IR_ON_RIGHT; 
			else 
				irFlag |= IR_STATE_RIGHT;
		}  
		else
		{ 
			if ( !(irFlag & IR_STATE_RIGHT) )  
				irFlag &= ~IR_ON_RIGHT; 
			else 
				irFlag &= ~IR_STATE_RIGHT;
		} 
	}
}

UInt8
irOn(UInt8 i)
{
	return irFlag & (i & 0x3);
}
