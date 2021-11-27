#include <pic.h>
#include <sys.h>

#include "interrupt.h"


DECLARE_INT_FLAGS;
TWEEK_MILLI(1,0);

void
init()
{
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all outputs
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;	
	T0IE = 0;
}

void timerCallback()
{
	RC4 = RC4 ? 0 : 1;
}

void
main()
{

	init();
//	initInterrupt(0,1);
	
	ei();	// enable interrupts
	while ( 1 )
	{

		CLRWDT();
		
		if ( oneKhz )
		{
			oneKhz = 0;
		
		}
	}
}
