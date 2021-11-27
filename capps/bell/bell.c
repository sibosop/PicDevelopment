#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"

DECLARE_INT_FLAGS;

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all output
	ADCON1 = 0x7;	// make port a digital
	TRISA = 0x1E;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
}

void
handleNoteOn(void)
{
	RC5 = 1;
	RA0 = mbyte2 ? 1 : 0;
}

void
main()
{

	init();
	initInterrupt(0,1);
	initSerial(1,0);
	initmparse();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC5 = 0;
		RC4 = 0;
		checkmparse();
		if ( oneKhz )
		{
			oneKhz = 0;
			RC4 = 1;
			RA1 = 1;
		}
	}
}
