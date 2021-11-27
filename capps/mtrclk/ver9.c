
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_9


#define RAMP_TIME 3
// Version 7 
// HP Input DB meter for hours
// HP output db meter for minutes
// Ludlow RF meter for seoncds
// small silver burlington for ticktock



void 
bumpHours()
{
	UInt8 hoursVal;
	++hours;
	if ( hours == 13 )
	{
		hours = 1;
	}
	if ( hours < 8 )
		hoursVal = (hours * 20) - 5;
	else
		hoursVal = hours * 20;
	writeD2A(HOURS_CHANNEL,hoursVal);
}



void
bumpMinutes(UInt8 hourFlag)
{	
	
	if ( ++minutes == 60 )	{			
		minutes = 0;
		if ( hourFlag ){
			bumpHours();
#ifdef USE_SERVO
			startChime(hours);
#endif
		}
	}
	else {
#ifdef USE_SERVO
		if ( hourFlag && !(minutes % 15) ){
			startChime(1);
		}			
#endif
	}
	writeD2A(MINUTES_CHANNEL,minutes*4);
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
#ifdef USE_SERVO
	stopServo();
#endif
	writeD2A(MINUTES_CHANNEL,0);
	writeD2A(HOURS_CHANNEL,20);
}

#define MAX_TICK	255
#define MIN_TICK	0

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
//	writeD2A(TICK_TOCK_CHANNEL,tickTock);
}

void
setSeconds()
{	
	if ( seconds > 30 )
		secondsVal = 30 - ( seconds % 30 );
	else
		secondsVal = seconds;
	writeD2A(SECONDS_CHANNEL,secondsVal*8);
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

	writeD2A(TICK_TOCK_CHANNEL,tickTockVal);
#endif
}

#endif
