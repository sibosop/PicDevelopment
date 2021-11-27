
#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
#include "a2d.h"

DECLARE_INT_FLAGS;
DECLARE_TIMERS(1);
DECLARE_A2D_VALS(3);
//TWEEK_MILLI(1,150);


//__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);


void
init()
{
	PORTC = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	TRISA = 0xFF;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;


}

#define GO		RB0 = 1
#define STOP 	RB0 = 0; 
#define FORWARD	RB1 = 1; RB2 = 0
#define BACKWARD RB1 = 0; RB2 = 1
#define BRAKE	RB1 = 1; RB2 = 1

typedef enum DIRS {
	Forward,
	Backward,
	Stop
} Dirs;


#define CHANGE_MARGIN	5

void
main()
{
	UInt8 val1 = 0;
	UInt8 val2 = 0;
	Dirs  dir = Stop;
	UInt8 margin = 0;

	init();
	initInterrupt(0,1);
	initTimers();
	initA2D();
	ei();	// enable interrupts

	while ( 1 )
	{
		CLRWDT();
		checkA2D();
		if ( oneKhz )
		{
			checkTimers();
			oneKhz = 0;
		}
	
		if ( timerReady(0) )
		{
			setTimer(0,500);
			val1 = getA2D(0);
			val2 = getA2D(1);
			margin = 0;
			switch ( dir )
			{
				default:
				case Stop:
					dir = val1 > val2 ? Forward : Backward;
					break;

				case Forward:
					if ( val1 < val2 )
						margin = val2 - val1;
					break;

				case Backward:
					if ( val1 > val2 )
						margin = val1 - val2;
					break;
			}
			if ( margin > CHANGE_MARGIN )
			{
				setTimer(0,1000);
				dir = Stop;
			}

			switch ( dir )
			{
				default:
				case Stop:
					STOP;
					break;
				case Forward:
					FORWARD; 
					GO;
					break;
				case Backward:
					BACKWARD;
					GO;
					break;
			}
		}	
	}
}
