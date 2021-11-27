
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_5
UInt16	hours;
UInt16	minutes;
UInt16	seconds;
UInt16	tickTock;
//#include "servo.h"
#define RAMP_TIME	20

void
setHours() 
{
	duties[HOURS_CHANNEL] = hours * 20;
}
void
setSeconds()
{
	if ( seconds > 30 ) {
		duties[SECONDS_CHANNEL] = 360 - ((seconds-30) * 12);
	} else {
		duties[SECONDS_CHANNEL] = seconds * 12;
	}
}

void
setMinutes()
{
	duties[MINUTES_CHANNEL] = minutes * 4;
}

// Version 5 eastman argentometer for hours
// square triplett meter for minutes
// square e.f. johnson seconds
// larger square dc volts triplett for tick tock


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
	
	if ( ++minutes == 60 )	{			
		minutes = 0;
		if ( hourFlag ){
			bumpHours();
		}
	}
}

UInt16	tickTockVal = 0;
UInt8	secondsFlag;

void
versionInit()
{
	tickTockVal = 0;
	minutes = 0;
	hours = 0;
	seconds = 0;
	secondsFlag = 0;
	setSeconds();
	setMinutes();
	setHours();
}

#define MAX_TICK	60
#define MIN_TICK	300

void
doClock()
{

	tickTock = tickTock==MAX_TICK ? MIN_TICK : MAX_TICK;


	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		bumpMinutes(1);
	}
	setMinutes();
	setHours();
}


void
doRamp()
{
	setTimer(RAMP_TIMER,RAMP_TIME);
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
