#include <pic.h>
#include <sys.h>
#include "types.h"

#include "seg.h"
#include "pseudorand.h"

Boolean	oneKhz;
#define QUANT	57
#define KHZT	6
UInt8	khzt;

void interrupt
interruptHandler()
{
	if ( T0IF  ) {
		T0IF = 0;
		TMR0 = QUANT;
		if ( !--khzt ) {
			oneKhz = 1;
			khzt = KHZT;
			RC4 = RC4 ? 0 : 1;
		}
	}
}




void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs, bit 2 data read
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x80;	// divide wdt by 2 disable pullups
	T0IE = 1;
	T0IF = 0;
	khzt = KHZT;
}

void
main()
{
	UInt16	segCount = 0;
	UInt16 	segDelay = 1;
	init();
	segInit();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{	
			oneKhz = 0;
			if ( !--segDelay ) {
				segDelay = 50;
				segDisplay(++segCount);
			}
		}		
	}
}

