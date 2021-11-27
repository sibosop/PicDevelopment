
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_5
//#include "servo.h"
#define RAMP_TIME	20

#error fuck
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
	writeD2A(HOURS_CHANNEL,hours*20);
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
	//writeD2A(MINUTES_CHANNEL,minutes * 4);
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
	//stopServo();
	//writeD2A(MINUTES_CHANNEL,0);
	//writeD2A(HOURS_CHANNEL,0);
}

#define MAX_TICK	80
#define MIN_TICK	55

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
	//writeD2A(SECONDS_CHANNEL,seconds * 4);
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

	//writeD2A(TICK_TOCK_CHANNEL,tickTockVal);
#endif
}

#endif
