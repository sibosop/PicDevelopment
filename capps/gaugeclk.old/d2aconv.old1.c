#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
#include "serial.h"

//__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);


DECLARE_INT_FLAGS;
DECLARE_TIMERS(1);
DISABLE_SERIAL;

#define	D2A_CLK	 RB1
#define D2A_LOAD RB2
#define D2A_DATA RB3

void
init()
{
	PORTB = 0xff;
	D2A_CLK	= 0;
	D2A_LOAD = 1;
	D2A_DATA = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x01;	// B is all output except for pin 0
	TRISA = 0xff;
	PORTA = 0;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 0;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON0 = 0;
	ADCON1 = 6;		// disable a/d ports/ make them digital
}

UInt8 tmp = 0;

void
writeD2A(UInt8 addr, UInt8 data)
{
	UInt8 mask;
	tmp = data;
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

#define SECONDS_CHANNEL	0
#define MINUTES_CHANNEL 1
#define HOURS_CHANNEL	2

#define CLOCK_MIN 55
#define CLOCK_MAX 255
#define CLOCK_RANGE (CLOCK_MAX - CLOCK_MIN)
#define CLOCK_INCR (CLOCK_RANGE/60)
#define CLOCK_INCR_HR (CLOCK_INCR)*5

UInt8 seconds = 0;
UInt8 minutes = 0;
UInt8 hours = 1;
UInt8 hrtmp = 0;
UInt8 pm = 0;

UInt8 prev_seconds = 255;
UInt8 prev_minutes = 255;
UInt8 prev_hours = 255;

void
updateClock()
{
	if (prev_seconds != seconds) 
	{
		prev_seconds = seconds;
		writeD2A(SECONDS_CHANNEL,CLOCK_MIN + seconds*CLOCK_INCR);
	}
	if (prev_minutes != minutes)
	{
		prev_minutes = minutes;
		writeD2A(MINUTES_CHANNEL,CLOCK_MIN + minutes*CLOCK_INCR);
	}

	//hours = hrtmp;
	if (prev_hours != hours) 
	{
		prev_hours = hours;
		writeD2A(HOURS_CHANNEL,CLOCK_MIN + hours*CLOCK_INCR_HR);
		//writeD2A(HOURS_CHANNEL,hours);
	}	
}

void
doClock()
{
   
	if ( ++seconds == 60 )
	{
		seconds = 0;
		if ( ++minutes == 60 )
		{
			minutes = 0;
			if ( ++hours == 13 )
			{
				pm = pm ? 0 : 1;
				hours = 1;
			}
		}
	}
	updateClock();
}

void
main()
{
	UInt8	i,j;
	UInt8	lineHz;
	UInt8	buttMask;
	UInt8	buttCount;
	init();
	initInterrupt(1,0);

	hours = 1;
	j = 0;
	lineHz = 0;
	buttMask = 0x00;

	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();		
		
		RC4 = 0;
		RC5 = 0;
		if ( pinInt )
		{
			
			if ( RA0 )
			{
				buttMask &= 0xfe;
			}
			else
			{
				if ( (buttMask & 1) != 1  )
				{
					buttCount = 60;
					buttMask |= 1; 
					++minutes;
					if ( minutes == 60 ) 
						minutes = 0;
					updateClock();
				}
				else
				{
						--buttCount;
						if ( !buttCount )
						{
							buttCount = 30;
							++minutes;
							if ( minutes == 60 ) 
								minutes = 0;
							updateClock();
						}
				}
			}
			

			pinInt = 0;
			if ( ++lineHz == 60 )
			{
				lineHz = 0;
				doClock();
			}
		}
#if 0
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();

			if ( timerReady(0) )
			{
				setTimer(0,1000);
#if 0
				j += 4;
				if ( j >= 240 ) j = 0;

				for ( i = 0; i < 3; ++i )
				{
					if ( i & 1 )
					{
						writeD2A(i,j);
					}
					else
					{
						writeD2A(i,255-j);
					}
				}
#endif
				doClock();
			} 
		}
#endif
	}
}
