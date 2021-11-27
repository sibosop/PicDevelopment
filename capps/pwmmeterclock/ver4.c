
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_4
#define RAMP_TIME	7
// Version 4 uses large round bruno meter for hours
// square weston for minutes
// small bruno for seconds
// round weston for tick tock
UInt16 seconds;
UInt16 minutes;
UInt16 hours;
UInt16 tickTock;


const UInt16 hourVals[] = 
{
	0,
	1,
	25,
	40,
	120,
	150,
	180,
	210,
	240,
	270,
	300,
	330,
	360,
} ;


void 
bumpHours()
{
	++hours;
	if ( hours == 13 )
	{
		hours = 1;
	}
}



void
bumpMinutes(UInt8 hourFlag)
{	
	
	if ( ++minutes == 60 )
	{			
		minutes = 0;
		if ( hourFlag )
			bumpHours();			
	}
}

UInt16	tickTockVal = 0;
UInt16	secondsVal  = 0;
UInt8	secondsFlag;


void
setMinutes() {
	duties[MINUTES_CHANNEL] = minutes * 6;
}

void
setHours() {
	duties[HOURS_CHANNEL] = hourVals[hours];
} 
void
versionInit()
{
	tickTockVal = 0;
	secondsVal  = 0;
	minutes = 0;
	hours = 0;
	seconds = 0;
	secondsFlag = 0;
	setMinutes();
	setHours();
}

#define MAX_TICK	350
#define MIN_TICK	10

void
doClock()
{

	tickTock = tickTock==MAX_TICK ? MIN_TICK : MAX_TICK;


	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		secondsVal = 0;
		bumpMinutes(1);
	}
	setHours();
}

void
setSeconds()
{
	if ( seconds > 30 )
		--secondsVal;
	else	
		++secondsVal;

	duties[SECONDS_CHANNEL] = secondsVal*12;
}

void
doRamp()
{
#if 1
	if ( tickTockVal == tickTock )
	{
		if ( secondsFlag )
		{
			secondsFlag = 0;
			setSeconds();
		}
		return;
	}
	secondsFlag = 1;

	if ( tickTockVal < tickTock )
		++tickTockVal;
	else
		--tickTockVal;	

	duties[TICK_TOCK_CHANNEL] = tickTockVal;
#endif
}

#endif
