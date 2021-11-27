
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"


#ifdef VERSION_6


#define RAMP_TIME 1
// Version 5 Unnamed cork lined ammeter for hours	
// round CCCP uAmmeter with square mounting for minutes
// square CCCP uAmmeter for seconds
// ludlow RF meter for tick tock

const UInt8 hourVals[] = {
0, 10, 25, 35, 45, 55, 65,
75, 85, 95, 105, 115, 125,
135, 145, 150, 160, 170, 180,
190, 200, 210, 220, 230, 240
};



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
	writeD2A(HOURS_CHANNEL,0);
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
	//writeD2A(HOURS_CHANNEL,hours);
}

void
setSeconds()
{
	if ( seconds > 30 )
		secondsVal = 30 - (seconds % 30);
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
