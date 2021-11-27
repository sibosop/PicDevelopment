
#include <pic.h>
#include <sys.h>

#include "interrupt.h"


DECLARE_INT_FLAGS;

__CONFIG(UNPROTECT & XT & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);


void
init()
{
	PORTC = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4/RC5 motor outputs
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
}


void
main()
{
	UInt8	nixie = 0;
	UInt16	counter = 0;
	init();
	initInterrupt(0,1);
	ei();	// enable interrupts

	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
			if ( ++counter == 1000 )
			{
				PORTB = nixie;		
				if ( ++nixie == 10 )
					nixie = 0;
			}
		}
	}
}
