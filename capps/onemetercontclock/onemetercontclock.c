#include <pic.h>
#include <sys.h>
#include "types.h"


#include "mclock.h"


const UInt8 millitCount = 120;
const UInt8 milliCount = 4;


UInt8	milli = 1;
Boolean	pinInt;
Boolean oneKhz;

UInt16	duties[NUM_METERS];
UInt16	dutyTimers[NUM_METERS];
UInt16	periodTimer;

UInt16	mramp;
UInt16	hramp;
Boolean	rflag;
//UInt8	sflag;
UInt16	sramp;
UInt16	*ep = dutyTimers+NUM_METERS;


void interrupt 
interruptHandler()
{
	UInt16	*dp;
	UInt16	*dup;
	UInt16 mask;
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
			PORTB &= ~0x0F;
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
	// one second timer interrupt
	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1IF = 0;
		pinInt = 1;
	}
}

void
initInterrupt()
{
	TMR0 = millitCount;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
	pinInt = 0;
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;
}


void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	pinInt = 0;
	oneKhz = 0;
}



void
main()
{
	init();
	initInterrupt();
	versionInit();
	initButtons();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			RC3 = RC3 ? 0 : 1;
			oneKhz = 0;
			doRamp();
			checkButtons();	
		}
		if (pinInt) {
			RC4 = RC4 ? 0 : 1;
			pinInt = 0;
			doClock();
			updateClock();
		}		
	}
}

