
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_3

// Version Three has the large black brono
// Small minutes and seconds brunos
// A weston ticker, which is damaged so 
// the code is modified to sweep only
// part of the meter

const UInt8 hourVals[] = 
{
	255,245,235,225,
	218,210,205,198,
	193,187,183,178,
	173,168,163,159,
	156,151,149,146,
	142,140,137,133
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
	writeD2A(HOURS_CHANNEL,hourVals[0]);
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
//	writeD2A(HOURS_CHANNEL,hours);
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
