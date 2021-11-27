#include <pic.h>
#include <sys.h>
#include <string.h>

#include "interrupt.h"
#include "timers.h"
#include "serial.h"

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);

#define MILLI	4
#define MILLIT	24

TWEEK_MILLI(MILLI,MILLIT);

DECLARE_INT_FLAGS;
DECLARE_TIMERS(1);
DISABLE_SERIAL;

#define	D2A_CLK	 RA0
#define D2A_LOAD RA1
#define D2A_DATA RA2


#define POWER_DETECT	RC3

//--------------------------------------
// Statics and defines for button logic
// 
#define		MAX_BUTTON 6
#define		BUTT_DOWN 1
#define		BUTT_UP   0
#define SECONDS_CHANNEL	0
#define MINUTES_CHANNEL 1
#define HOURS_CHANNEL	2

#if 0
#define CLOCK_MIN (UInt8)75
#define CLOCK_MAX (UInt8)255
#define CLOCK_RANGE ((UInt8)(CLOCK_MAX - CLOCK_MIN))
UInt8 CLOCK_INCR = (CLOCK_RANGE/60);
#define CLOCK_INCR_HR (CLOCK_INCR)*5
#else // 0
#define CLOCK_MIN (UInt8)0
#define CLOCK_MAX (UInt8)240
#define CLOCK_RANGE ((UInt8)(CLOCK_MAX - CLOCK_MIN))
UInt8 CLOCK_INCR = (CLOCK_RANGE/60);
#define CLOCK_INCR_HR (CLOCK_INCR)*5
#endif // 0

UInt8 seconds = 0;
UInt8 minutes = 0;
UInt8 hours = 1;
UInt8 hrtmp = 0;
UInt8 pm = 0;
UInt8 calibVal = 0;	// for CLKMODE_CALIB

UInt8 prev_seconds = 255;
UInt8 prev_minutes = 255;
UInt8 prev_hours = 255;

typedef	struct Button_s {
		UInt8 buttState;
		UInt8 buttHold;
		UInt8 buttCount;
		} Button;

Button	Buttons[MAX_BUTTON];

#if 0
#define		BTN_SEC_ZERO 0
#define		BTN_MIN_UP 1
#define		BTN_MIN_DN 2
#define		BTN_HR_UP 3
#define		BTN_HR_DN 4
#define		BTN_MODE 5
#else
UInt8		BTN_SEC_ZERO = 0;
UInt8		BTN_MIN_UP = 1;
UInt8		BTN_MIN_DN = 2;
UInt8		BTN_HR_UP = 3;
UInt8		BTN_HR_DN = 4;
UInt8		BTN_MODE = 5;
#endif

//--------------------------------------
// Clock mode: calibrate or run

#define		CLKMODE_RUN		0
#define		CLKMODE_CALIB	1
UInt8		ClockMode = CLKMODE_RUN;


extern UInt8 milli;

void
initButton( Button *btn )
{
	memset( btn, 0, sizeof( *btn ) );
}

void
init()
{
	UInt8 i;
	PORTB = 0x00;
	PORTA = 0x00;
	PORTC = 0x00;
	
	TRISC = 0x08;	// make C3 input for power detect and Rcv, RC4 is clockout
	TRISB = 0x3F;	// Input pins for buttons
	TRISA = 0x00;	// make port a all outputs (for D/A)
	D2A_CLK	= 0;
	D2A_LOAD = 1;
	D2A_DATA = 0;
	
	OPTION = 0X0F;	// divide timer0 by 2 enable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON0 = 0;
	ADCON1 = 6;		// disable a/d ports/ make them digital

//	milli = milliCount;
	T0IE = 1;	// enable timer 0 interrupt
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;	
	for (i=0; i < MAX_BUTTON; i++)
	{
	    initButton( &Buttons[ i ] );
	}
	calibVal = 0;
	milli = 1;
}

UInt8 tmp = 0;

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
	D2A_DATA = 1; // data double bit 0 = normal 1 = double	
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
updateClock()
{
	if (1 || prev_seconds != seconds) 
	{
		
		prev_seconds = seconds;
		writeD2A(SECONDS_CHANNEL,CLOCK_MIN + ((seconds) * CLOCK_INCR));
	}
	if (1 || prev_minutes != minutes)
	{
		prev_minutes = minutes;
		writeD2A(MINUTES_CHANNEL,CLOCK_MIN + (minutes+1)*CLOCK_INCR);
	}

	//hours = hrtmp;
	if (1 || prev_hours != hours) 
	{
		prev_hours = hours;
		writeD2A(HOURS_CHANNEL,CLOCK_MIN + hours*CLOCK_INCR_HR);
		//writeD2A(HOURS_CHANNEL,hours);
	}	
}

void
doClock()
{
 	//RC4 = RC4 ? 0 : 1;
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
	//updateClock();
}

UInt8 
incrModCounter( UInt8 count, UInt8 minval, UInt8 maxval, Int8 incr )
{

	if (count == minval && incr < minval)
	{
		count = maxval;	// wrap backwards
	}
	else
	{
		count += incr;	// incr may be < minval
		if (count >= maxval + minval)
		{
			count = minval; // wrap forwards
		}
	}
	return count;
}

UInt8
getButtStates()
{
	UInt8 i;
	UInt8 rval = 0;
	Button *btn;

	Buttons[ BTN_SEC_ZERO ].buttState	= RB3 ? BUTT_UP : BUTT_DOWN;
	Buttons[ BTN_MIN_UP ].buttState		= RB4 ? BUTT_UP : BUTT_DOWN;
	Buttons[ BTN_MIN_DN ].buttState		= RB1 ? BUTT_UP : BUTT_DOWN;
	Buttons[ BTN_HR_UP ].buttState		= RB5 ? BUTT_UP : BUTT_DOWN;
	Buttons[ BTN_HR_DN ].buttState		= RB2 ? BUTT_UP : BUTT_DOWN;
	Buttons[ BTN_MODE ].buttState		= RB0 ? BUTT_UP : BUTT_DOWN;

	
	btn = Buttons;
	for (i=0; i < MAX_BUTTON; i++, btn++)
	{
		
		if (btn->buttState == BUTT_UP)
		{
			btn->buttHold = 0;
		}
		else
		{
			if (! btn->buttHold)
			{
				btn->buttCount = 60;
				btn->buttHold = 1;
				rval = 1;
			}
			else
			{
				btn->buttCount--;
				if ( !btn->buttCount )
				{
					btn->buttCount = 10;
					rval = 1;
				}
			}
		}
	}
	return rval;
}

void
setAllChannels( UInt8 val )
{
	writeD2A(SECONDS_CHANNEL,val);
	writeD2A(MINUTES_CHANNEL,val);
	writeD2A(HOURS_CHANNEL,val);
}

void
handleButtStates()
{
	UInt8 doUpdate = 0;
	UInt8 tmp = calibVal;

#if 0
	if (Buttons[ BTN_MODE ].buttState == BUTT_DOWN)
	{
		// toggle run mode
		ClockMode = (ClockMode == CLKMODE_RUN) ? CLKMODE_CALIB : CLKMODE_RUN;
		if (ClockMode == CLKMODE_RUN)
		{
			setAllChannels(0);
			doUpdate++;
		}
	}
#endif

	if (ClockMode == CLKMODE_CALIB)
	{
		if (Buttons[ BTN_SEC_ZERO ].buttState == BUTT_DOWN)
		{
			// toggle display between min and max
			calibVal = (calibVal) ? 0 : 240;
		}
		else if (Buttons[ BTN_MIN_DN ].buttState == BUTT_DOWN ||
		         Buttons[ BTN_HR_DN ].buttState == BUTT_DOWN)
		{
			// bump everthing up
			calibVal = incrModCounter( calibVal, 0, 240, -4 );
		} 
		else if (Buttons[ BTN_MIN_UP ].buttState == BUTT_DOWN ||
		         Buttons[ BTN_HR_UP ].buttState == BUTT_DOWN)
		{
			// bump everthing down
			calibVal = incrModCounter( calibVal, 0, 240, 4 );
		}
		if ( tmp != calibVal )
			setAllChannels( calibVal );
		return;
	}


	if (Buttons[ BTN_SEC_ZERO ].buttState == BUTT_DOWN)
	{
		seconds = 0;
		writeD2A(SECONDS_CHANNEL,seconds);
		// doUpdate++;
	}

	if (Buttons[ BTN_MIN_UP ].buttState == BUTT_DOWN)
	{
#if 0
		minutes = incrModCounter( minutes, 0, 240, 4 );
		writeD2A(MINUTES_CHANNEL,minutes);
#else
		minutes = incrModCounter( minutes, 0, 59, 1 );
		doUpdate++;
#endif
	}
	else if (Buttons[ BTN_MIN_DN ].buttState == BUTT_DOWN)
	{
#if 0
		minutes = incrModCounter( minutes, 0, 240, -4 );
		writeD2A(MINUTES_CHANNEL,minutes);
#else
		minutes = incrModCounter( minutes, 0, 59, -1 );
		doUpdate++;
#endif
	}

	if (Buttons[ BTN_HR_UP ].buttState == BUTT_DOWN)
	{
#if 0
		hours = incrModCounter( hours, 0, 240, 4 );
		writeD2A(HOURS_CHANNEL,hours);
#else
		hours = incrModCounter(hours, 1, 12, 1 );
		doUpdate++;
#endif
	}
	else if (Buttons[ BTN_HR_DN ].buttState == BUTT_DOWN)
	{
#if 0
		hours = incrModCounter( hours, 0, 240, -4 );
		writeD2A(HOURS_CHANNEL,hours);
#else
		hours = incrModCounter( hours, 1, 12, -1 );
		doUpdate++;
#endif
	}

	if (doUpdate)
	{
		updateClock( );
	}
		
}

void powerDown()
{
	setAllChannels(0);
	TRISB = 0;
	PORTB = 0;
	PORTC = 0;
	PORTA = 0;
}

void powerUp()
{
	TRISB = 0xFF;	// reset all port b pins to input
	setAllChannels(0);
}

void
main()
{
	UInt8	debounce = 0;
	UInt16   state = 0;
	UInt8	lastClockMode = CLKMODE_RUN;
	//UInt8	buttCount;
	init();
	
	hours = 1;

	RC4 = 0;
	setAllChannels(0);
	RC4 = 1;
	ei();
	while ( 1 )
	{
		CLRWDT();			
		
		if ( oneKhz )
		{
			ClockMode = RB0 ? CLKMODE_RUN : CLKMODE_CALIB;
			if ( lastClockMode != ClockMode )
			{
				lastClockMode = ClockMode;
				if ( ClockMode == CLKMODE_RUN )
					setAllChannels(0);
			}
	
			oneKhz = 0;
			if ( state < 500 )
			{
			//	RC4=RC4 ? 0 : 1;
				++state;
				continue;
			}
			if ( ++debounce == 20 )
			{
				
				debounce = 0;
				if (getButtStates( ))
					handleButtStates( );
				if (ClockMode == CLKMODE_RUN)
					updateClock();
			}
			
		}	
		RC5 = RC5 ? 0 : 1;
		if ( !POWER_DETECT )
		{
			//RC5 = RC5 ? 0 : 1;
			powerDown();
			while ( !POWER_DETECT )
			{
				RC5 = RC5 ? 0 : 1;
				SLEEP();
			}
			powerUp();
			state = 0;
		}
	}
}
