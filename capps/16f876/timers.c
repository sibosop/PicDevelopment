#include "timers.h"


void initTimers(void)
{
	UInt8 i;
	for( i = 0; i < numTimers; ++i )
		Timers[i] = 0;
	
}

void checkTimers(void)
{
	UInt8 timer;
	for ( timer = 0; timer < numTimers; ++timer) 
	{
		if ( Timers[timer] )
		{
			--Timers[timer];
		}
	}
}

UInt8 
timerReady(UInt8 timer)
{
	return Timers[timer] ? 0 : 1;
}

void
setTimer(UInt8 timer, UInt16 time )
{
	Timers[timer] = time;
} 
