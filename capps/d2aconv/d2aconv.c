#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
//#include "serial.h"
//#include "midi.h"
//#include "a2d.h"

DECLARE_INT_FLAGS;
DECLARE_TIMERS(1);
//DECLARE_A2D_VALS(2);

#define	D2A_CLK	 RB0
#define D2A_LOAD RB1
#define D2A_DATA RB2

void
init()
{
	PORTB = 0xff;
	D2A_CLK	= 0;
	D2A_LOAD = 1;
	D2A_DATA = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all output
	TRISA = 0x00;
	PORTA = 0;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 0;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
}

#if 0
Boolean clockFlag;
#endif

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
	D2A_DATA = 1;	
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


void
main()
{
	UInt8	i,j;
#if 0
	int clockCount = 0;
	int	vel = 100;
#endif
	init();
	initInterrupt(0,1);

#if 0
	initSerial(1,1);
	initmparse();
	initA2D();
#endif
#if 0
	j = 67;
#else
	j = 0;
#endif
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC4 = 0;
		RC5 = 0;
#if 0
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
#endif
		
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();

			if ( timerReady(0) )
			{
				setTimer(0,1000);
#if 0
				j += 3;
				if ( j >= 250 ) j = 70;
#else
				j += 4;
				if ( j >= 240 ) j = 0;
#endif
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
			} 
		}
	}
}
