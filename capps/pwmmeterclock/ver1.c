
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"
#ifdef VERSION_1


UInt16 tickTockVal = 0;
UInt8  secondsFlag = 0;



// Version One has the Brown Triplett hour
// A Simpson Volt Minutes
// A micron Seconds
// A HP OUTPUT voltage meter
void
setHours() {
	UInt16 tmp = hours;
	duties[HOURS_CHANNEL] = tmp * 30;
}

void 
bumpHours()
{
	++hours;
	if ( hours == 13 )
	{
		hours = 1;
	}
}
const UInt16 minuteVals[] = 
{
	0,4,8,12,16  // 0
	,21,26,31,36,42 // 5
	,47,50,53,58,62 // 10
	,66,69,72,75,79 // 15
	,83,86,89,92,95 // 20
	,98,100,102,104,107 // 25
	,110,113,116,120,124 // 30
	,128,131,134,137,141 // 35
	,144,147,150,153,156 // 40
	,159,162,165,168,171 // 45
	,174,176,178,180,183 // 50
	,185,187,189,191,193 // 55
	,194				// 60
};
void
setMinutes() {
	duties[MINUTES_CHANNEL] = HIGH_SET - minuteVals[minutes];
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

void
setSeconds() {
	UInt16 tmp = seconds;
	duties[SECONDS_CHANNEL] = tmp * 6;
}

void
doClock()
{
	tickTock = (tickTock==LOW_TICK) ? HIGH_TICK : LOW_TICK;
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
  setTimer(RAMP_TIMER,TT_DELAY);
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
}

void
versionInit(){
	hours = 1;
	minutes = 0;
	seconds = 0;
	setHours();
	setMinutes();
	setSeconds();
}

#endif
