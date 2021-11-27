#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "playnote.h"

DECLARE_TIMERS(4);
DECLARE_INT_FLAGS;

#if defined(_16F876)
#include "serial.h"
#endif


DECLARE_QUEUES(16,48);

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);



void checkTransmit(){}
void checkReceive(){}


void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;


}






void
main()
{
	UInt8	i;
	init();
	initInterrupt(0,1);
	initTimers();
	initRandom();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
		 	checkTimers();
		}
		if ( checkTimer(0) )
		{
			setTimer(0,1000);
			RB0 = RB0 ? 0 : 1;
		}
	}
}
