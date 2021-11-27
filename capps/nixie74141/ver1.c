
#include <pic.h>
#include <sys.h>
#include "types.h"
#include "a2d.h"
#include "nixie74141.h"
//Boolean hourFlag;
UInt8	hours;
UInt8	minutes;
UInt8	seconds;
UInt16	tickTock;
UInt16 tickTockVal = 0;
UInt8  secondsFlag = 0;



#ifdef VERSION_1

// Version One has the Nixie Hours Minutes
//
void
setHours() {
	nixieVals[NIXIE_HOUR_LSB] = hours % 10;
	nixieVals[NIXIE_HOUR_MSB] = hours / 10;
}


bank3 UInt16 adjust;
bank3 UInt8 up;
bank3 UInt16 overflow = 0;
bank3 UInt16 secs;
bank3 UInt8 adjustCount;
int
secondsAdjust() {
	adjust = getA2D(0);
	if ( adjust >= 0x200 ) {
		adjust -= 0x200;
		up = 1;
	} else {
		adjust = 0x200 - adjust;
		up = 0;
	}
	secs = adjust & 0x300;
	overflow += adjust & 0xff;
	secs = (secs + overflow);
	secs &= 0x300;
	overflow &= 0xff;
	secs >>= 8;
	if ( up ) 
		seconds += secs;
	else	
		seconds -= secs;
	adjustCount = 30;
	return 0;
}
void 
bumpHours()
{
	++hours;
	if ( hours == 13 )
	{
		hours = 1;
	}
	//hourFlag = 1;
}

void
setMinutes() {
	nixieVals[NIXIE_MIN_LSB] = minutes % 10;
	nixieVals[NIXIE_MIN_MSB] = minutes / 10;
}

void
bumpMinutes(/*UInt8 hourFlag*/)
{		
	if ( ++minutes == 60 )
	{			
		minutes = 0;
	//	if ( hourFlag )
			bumpHours();			
	}

}

void
setSeconds() {
	UInt16 tmp = seconds;
	duty1 = tmp * 6;
}

void
doClock()
{
	tickTock = (tickTock==LOW_TICK) ? HIGH_TICK : LOW_TICK;
	if ( ++seconds == 60 ) 
	{
		seconds = 0;
		bumpMinutes(/*1*/);
	}
	if ( !adjustCount ) {	
		if ( minutes == 32 && seconds == 30 )
			secondsAdjust();
	} else {
		--adjustCount;
	}
	setSeconds();
	setMinutes();
	setHours();
	//secondsAdjust();
}

UInt16 rampDelay;

void
doRamp()
{
  if ( --rampDelay )
		return;
  rampDelay = TT_DELAY;
  if ( tickTockVal == tickTock )
  {
    if ( secondsFlag )
    {
      secondsFlag = 0;
      setSeconds();
	  setMinutes();
	  setHours();
    }
    return;
  }
  secondsFlag = 1;

  if ( tickTockVal < tickTock )
    ++tickTockVal;
  else
    --tickTockVal;
  duty0= tickTockVal;
}

void
versionInit(){
	hours = 10;
	minutes = 16;
	seconds = 0;
	setHours();
	setMinutes();
	setSeconds();
	rampDelay = TT_DELAY;
}

#endif
