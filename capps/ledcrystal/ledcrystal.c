#include <pic.h>
#include <sys.h>
#include "types.h"


#include "leds.h"
#include "pseudorand.h"
#include "ledcrystal.h"
#include "interrupt.h"


DECLARE_INT_FLAGS;
TWEEK_MILLI(18,11);

void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	TRISA = 0x00;	// make A outputs
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


void
main()
{
	init();
	initInterrupt(0,1);
	
	ledInit();
	ei();	// enable interrupts


	while ( 1 )
	{
		CLRWDT();
		
		if ( oneKhz )
		{
			oneKhz = 0;
			RB0 = RB0 ? 0 : 1;
			checkLeds();
		}
	}
}

