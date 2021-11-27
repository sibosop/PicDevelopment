#include <pic.h>
#include <sys.h>
#include "types.h"
#include "pseudorand.h"





const UInt8 millitCount = 0;
const UInt8 milliCount = 17;


UInt8	milli = 1;

Boolean oneKhz;
#define NUM_METERS	1
#define PERIOD		180
UInt8	duties[NUM_METERS];
UInt8	dutyTimers[NUM_METERS];
UInt8 	periodTimer;

UInt8	mramp;
UInt8	hramp;
Boolean	rflag;
//UInt8	sflag;
UInt8	sramp;
UInt8	*ep = dutyTimers+NUM_METERS;


void interrupt 
interruptHandler()
{
	UInt8	*dp;
	UInt8	*dup;
	UInt8 mask;
	// test timer 0 interrupt
	if ( T0IF )
	{	
		T0IF = 0;
		TMR0 = millitCount;
		if ( !(--milli) )
		{
			milli = milliCount;
			oneKhz = 1;
			
		}
	
		if ( !(--periodTimer)  )
		{
			periodTimer = PERIOD;
			PORTB &= ~0x01;
			dp = dutyTimers;
			dup= duties;
			for(mask = 1; dp<ep; mask <<= 1) {
				if ( *dup )
					PORTB |= mask;
				*dp++ = *dup++;	
			}
		}
		else
		{
			for( dp = dutyTimers, mask=1;
					dp < ep;
					mask<<=1,
					++dp)
			{
				if ( PORTB & mask )
				{
					if (!(--(*dp)))
						PORTB &= ~mask;
				}
			}
		}	
	}	
}

void
initInterrupt()
{
	TMR0 = millitCount;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
	
}


void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	TRISA = 0x00;
	TRISA4 = 1;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	oneKhz = 0;
}
UInt8 gbVal;
UInt16 gbTimer;
Boolean good;
UInt8 debounce;
#define GOOD_REG	RB2
#define REDO_WAIT_TIME	10000
#define PROX	RA4
#define LED		RB4
void
checkMotionSensor() {
	if ( --gbTimer ) {
		if ( gbVal && !PROX ) {
			gbTimer = REDO_WAIT_TIME;
		} 
		return;
	}

	if ( PROX ) {
		gbTimer = 1;
		LED = 0;
		gbVal = 0;
	} else {	
		if ( duties[0] ) {
			gbTimer = 10;
		} else {	
			LED = 1;
			gbVal = pseudoRand(0,PERIOD);
			good = pseudoRand(0,2);
			gbTimer = REDO_WAIT_TIME;
		}	
	}
}


void
main()
{
	UInt16 t = 1;
	pseudoSeed(666);
	init();
	initInterrupt();
	ei();	// enable interrupts
	gbTimer = 1;
	gbVal = 0;
	good = 1;
	duties[0] = 0;
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			RC3 = RC3 ? 0 : 1;
			oneKhz = 0;
			checkMotionSensor();
			if ( !--t ) {
				t = 30;
				if ( GOOD_REG != good ) {
					if ( duties[0] )
						--duties[0];
					if ( !duties[0] ) {
						GOOD_REG = good;
					}
				} else {
					if ( duties[0] < gbVal ) {
						++duties[0];
					} 
					if ( duties[0] > gbVal ) {
						--duties[0];
					}
				}
			}
		}
	}
}

