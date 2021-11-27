

#include <pic.h>
#include <sys.h>

#include "types.h"
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
	TRISB = 0x00; // RB5 - minutes RB0 - 60HZ  RB1,2,3 -D2A outputs
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
	initInterrupt(1,1);
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC5 = 0;

		if ( oneKhz )
		{
			oneKhz = 0;
			if ( RC3 ) {
				RC3 = 0;
			} else {
				RC3 = 1;
			}	
			checkTimers();
			if ( timerReady(0) ) {
				if ( on ) {
					setTimer(0,OFF_TIME(onTime));
					RC4 = 0;
					on = 0;
				} else {
					on = 1;
					setTimer(0,ON_TIME(onTime));
					RC4=1;
				}
			}
		
			if ( timerReady(1) ) {
			

				setTimer(1,1000);
				if ( ++onTime == INTERVAL ) 
					onTime = 0;
			}
		}
	}
} 
