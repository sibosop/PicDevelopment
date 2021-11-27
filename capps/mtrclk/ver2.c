
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_2

// Version Two has the Weston 24 hour
// Micron Minutes
// A forward-backward Electel Seconds
// A Black Faced GE TickTock


void 
bumpHours()
{
	++hours;
	if ( hours == 24 )
	{
		hours = 0;
	}
	writeD2A(HOURS_CHANNEL,hours * 10);
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
	writeD2A(MINUTES_CHANNEL,240-(minutes * 3));

}

UInt8	tickTockVal = 0;
UInt8	secondsVal  = 0;
UInt8	secondsFlag;

void
versionInit()
{
	tickTockVal = 0;
	secondsVal  = 0;
	minutes = 0;
	hours = 0;
	seconds = 0;
	secondsFlag = 0;
	writeD2A(MINUTES_CHANNEL,240-(minutes * 3));
	writeD2A(HOURS_CHANNEL,hours * 10);
}


void
doClock()
{

	tickTock = tickTock ? 0 : 255;
	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		secondsVal = 255;
		bumpMinutes(1);
	}
}

void
setSeconds()
{
	if ( seconds > 30 )
		--secondsVal;
	else	
		++secondsVal;

	writeD2A(SECONDS_CHANNEL,secondsVal * 8);
}

void
doRamp()
{
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

	writeD2A(TICK_TOCK_CHANNEL,tickTockVal);
}

#endif
