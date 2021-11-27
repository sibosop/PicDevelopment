#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "a2d.h"

DECLARE_INT_FLAGS;
TWEEK_MILLI(3,120);

DECLARE_TIMERS(2);
void
init()
{
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0xFF;	// B is all inputs
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
}




void
main()
{

	init();
	initInterrupt(0,1);
	initTimers();
	ei();	// enable interrupts
	RC4 = 0;
	RC5 = 0;
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
		}
		if ( timerReady(0) )
		{
			if ( !RC4 )
			{
				if ( RC5 )
				{
					RC5 = 0;
					setTimer(0,10);
				}
				else
				{
					RC4 = 1;
					setTimer(0,2);
				}
			}
			else if ( !RC5 )
			{
				RC4 = 0;
				RC5 = 1;
				setTimer(0,2);
			}
		}
	}
}
