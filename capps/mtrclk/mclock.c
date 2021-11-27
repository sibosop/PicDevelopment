

#include <pic.h>
#include <sys.h>

#include "types.h"
#include "serial.h"
#include "interrupt.h"
#include "timers.h"
#include "mclock.h"
#if defined(VERSION_5)
#include "servo.h"
#endif

DISABLE_SERIAL
DECLARE_INT_FLAGS;
DECLARE_TIMERS(3);

UInt8	tickTock;
UInt8	seconds;
UInt8	minutes;
UInt8	hours;
UInt8	pm;

void
init()
{
	PORTB = 0xff;
	D2A_CLK	= 0;
	D2A_LOAD = 1;
	D2A_DATA = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	
	// 
	TRISB = 0x21; // RB5 - minutes RB0 - 60HZ  RB1,2,3 -D2A outputs
	TRISA = 0x01; // RA0 - hours
	PORTA = 0;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 0;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	ADCON0 = 0;
	ADCON1 = 6;		// set all a/d ports to digital
	INTEDG = 1;
	tickTock = 0;
	seconds = 0;
	minutes = 0;
	hours = 1;
	pm = 0;
	PM_LIGHT = 0;
}


void
writeD2A(UInt8 addr, UInt8 data)
{
	UInt8 mask;
	for ( mask = 0x4; mask; mask >>= 1 )
	{
		D2A_CLK = 1;
		D2A_DATA = (addr & mask) ? 1 : 0;
		D2A_CLK = 0;
	}
	D2A_CLK  = 1;
	D2A_DATA = 0;	
	D2A_CLK  = 0;
	for ( mask = 0x80; mask; mask >>=1 )
	{
		D2A_CLK = 1;
		D2A_DATA = (data & mask) ? 1 : 0;
		D2A_CLK = 0;
	}
	D2A_LOAD = 0;
	D2A_LOAD = 1;
}

UInt8 	testminutes = 0;
UInt8   testhours = 0;


void
handleBut(UInt8 butVal)
{}

void
main()
{
	UInt8	hourDown = 0;
	UInt8	minuteDown = 0;
	UInt8	tmp = 0;
	
	UInt8	hzcnt = 0;
	init();
	versionInit();
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
			if ( timerReady(RAMP_TIMER) )
			{
				
				doRamp();
			}
			if ( timerReady(HOUR_TIMER) )
			{
				if ( !HOUR_SET )
				{
					++hourDown;
					switch ( hourDown )
					{
						case 0:
							break;
						case 1:
							setTimer(HOUR_TIMER,100);
							break;

						case 2:
							bumpHours();
							break;
	
						default:
							--hourDown;
							break;
					}
				}
				else if ( HOUR_SET )
				{
					hourDown = 0;
				}
			}
			if ( timerReady(MINUTE_TIMER) )
			{
				if ( !MINUTE_SET )
				{
					++minuteDown;
					switch ( minuteDown )
					{
						case 1:
							setTimer(MINUTE_TIMER,100);
							break;

						case 2:
							bumpMinutes(0);
							break;
				
						default:
							--minuteDown;
							break;
					}
				}
				else if ( MINUTE_SET )
				{
					minuteDown = 0;
				}
			}
		}
		
		if ( pinInt )
		{
			pinInt = 0;
			hzcnt = 0;
			doClock();
#if defined(USE_SERVO)
			chimeTest();
#endif
		}
	}
}
