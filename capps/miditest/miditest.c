#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "a2d.h"

DECLARE_INT_FLAGS;
DECLARE_A2D_VALS(2);


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
	int clockCount = 0;
	int	vel = 100;

	init();
	initInterrupt(0,1);
	initSerial(1,1);
	initmparse();
	initA2D();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC4 = 0;
		RC5 = 0;
		if ( clockFlag )
		{
			clockFlag = 0;
			++clockCount;
			if ( clockCount == 24 )
			{
				clockCount = 0;
				vel = vel ? 0 : getA2D(1);
				putNote(60,vel,0);
				
			}
		}
		checkmparse();
		checkA2D();
		if ( oneKhz )
		{
			RC5 = 0;
			oneKhz = 0;
			RC4 = 1;
		}
	}
}
