
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

const UInt8 hourVals[] = 
{
	255,241,231,223,
	216,208,203,196,
	189,182,176,171,
	168,164,158,155,
	151,148,144,140,
	136,133,130,127
} ;


void 
bumpHours()
{
	++hours;
	if ( hours == 24 )
	{
		hours = 0;
	}
	writeD2A(HOURS_CHANNEL,hourVals[hours]);
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
	writeD2A(MINUTES_CHANNEL,minutes * 4);

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
	writeD2A(MINUTES_CHANNEL,minutes*4);
	writeD2A(HOURS_CHANNEL,hourVals[hours]);
}

#define MAX_TICK	170
#define MIN_TICK	50

void
doClock()
{

	tickTock = tickTock==MAX_TICK ? MIN_TICK : MAX_TICK;


	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		secondsVal = 255;
		bumpMinutes(1);
	}
	//writeD2A(HOURS_CHANNEL,hours);
}

void
setSeconds()
{
	if ( seconds > 30 )
		--secondsVal;
	else	
		++secondsVal;

	writeD2A(SECONDS_CHANNEL,secondsVal*8);

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

	writeD2A(TICK_TOCK_CHANNEL,tickTockVal);
#endif
}

#endif
