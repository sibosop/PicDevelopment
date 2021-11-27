#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"

DECLARE_INT_FLAGS;

DECLARE_TIMERS(2);

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all output
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port

}

Boolean clockFlag;

void
main()
{
	UInt8	mask;
	init();
	initInterrupt(0,1);
	initSerial(1,1);
	initmparse();
	initTimers();
	ei();	// enable interrupts
	mask = 1;
	while ( 1 )
	{
		CLRWDT();
		checkmparse();
#if 0
		if ( timerReady(0) )
		{
			setTimer(0,500);
			PORTB = 0;
			RC4 = 0;
			RC5 = 0;
			if ( mask & 0x40 )
			{
				RC4 = 1;
			}
			else if ( mask & 0x80 )
			{
				RC5 = 1;
			}
			else
			{
				PORTB = mask;
			}
			
			if ( mask & 0x80 )
				mask = 1;
			else
				mask <<= 1;
		}
#endif
		if ( oneKhz )
		{
			checkTimers();
			oneKhz = 0;
		}
	}
}
