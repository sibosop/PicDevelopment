
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_1

// Version One has the Brown Triplett hour
// A Simpson Volt Minutes
// A micron Seconds
// A a 0-20 ticktock

// this is for the 'micro meter'
// the meter reads from right to left
// and is not linear
const UInt8 secVals[] =
{
// 0-5
255,252,249,246,243,
// 5-10
240,237,234,231,228,
// 10-20
225,221,219,216,213,210,207,204,201,198,
// 20-30
195,193,190,188,186,184,183,182,181,180,
// 30-40
178,176,173,170,167,165,163,161,158,156,
// 40-50
154,152,150,148,145,143,141,139,136,134,
// 50-60
132,130,128,126,125,124,123,122,121,120,
//
118
};
void 
bumpHours()
{
	++hours;
	if ( hours == 12 )
	{
		pm = pm ? 0 : 1;
		PM_LIGHT = pm;
	} 
	if ( hours == 13 )
	{
		hours = 1;
	}
	writeD2A(HOURS_CHANNEL,hours * 20);
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
	writeD2A(MINUTES_CHANNEL,minutes*4);
}


void
doClock()
{
	tickTock = tickTock ? 0 : 255;
#if 1	
	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		bumpMinutes(1);
	}

					
	writeD2A(TICK_TOCK_CHANNEL,tickTock);
	writeD2A(SECONDS_CHANNEL,secVals[seconds]);
//	writeD2A(SECONDS_CHANNEL,testhours);
// 	writeD2A(MINUTES_CHANNEL,minutes*4);
//	writeD2A(HOURS_CHANNEL,hours * 20);
#else
	writeD2A(TICK_TOCK_CHANNEL,tickTock);
	writeD2A(SECONDS_CHANNEL,tickTock ? 127 : 67);
	writeD2A(MINUTES_CHANNEL,tickTock);
	writeD2A(HOURS_CHANNEL,tickTock ? 0 : 12 );
#endif
}

void
doRamp(){}

void
versionInit(){}

#endif
