

#include <pic.h>
#include <sys.h>

#include "types.h"
#include "oneWireTemp.h"
#include "interrupt.h"
#include "timers.h"
DECLARE_INT_FLAGS;
DECLARE_TIMERS(3);
#define MILLI	1
#define MILLIT	0xA0
const UInt8 milliCount;
const UInt8 millitCount;
TWEEK_MILLI(MILLI,MILLIT);
void
init()
{
	PORTB = 0xff;
	TRISC = 0x0;
	
	// 
	TRISB = 0x21; // RB5 - minutes RB0 - 60HZ  RB1,2,3 -D2A outputs
	TRISA = 0x01; // RA0 - hours
	PORTA = 0;
	OPTION = 0x88;	// divide timer0 by 1 disable pullups
	PEIE = 0;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	ADCON0 = 0;
	ADCON1 = 6;		// set all a/d ports to digital
	INTEDG = 1;
}
UInt16	onTime = 1;
#define INTERVAL	(UInt16)50
#define PULSE_TIME	200
#define ON_TIME(x)	(x)
#define OFF_TIME(x)	(INTERVAL-(x))
void
main()
{
	UInt8	on = 0;
	
	init();
	initOneWireTemp();
	initInterrupt(1,1);
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC5 = 0;

		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
			checkOneWireTemp();
		}
	}
} 
