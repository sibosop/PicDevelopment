#include <pic.h>
#include <sys.h>
#include "types.h"
#include "sumosup.h"
#include "timers.h"

bank1 UInt16 currentSpeedRight = NORMAL_SPEED;
bank1 UInt16 currentSpeedLeft  = NORMAL_SPEED;
bank1 UInt16 speedGoalLeft     = NORMAL_SPEED;
bank1 UInt16 speedGoalRight    = NORMAL_SPEED;
bank1 UInt16 speedInterval	   = DEFAULT_SPEED_INTERVAL;

typedef struct _SpeedInfo {
	UInt16	currentSpeed;
	UInt16	speedGoal;
	UInt16	speedInterval;
} SpeedInfo;

bank1 SpeedInfo speedInfo[2];

const enum Timers speedTimers[2] = 
{
	SpeedTimerLeft,
	SpeedTimerRight,
} ;



void
initSpeed()
{
	UInt8 i;
	bank1 SpeedInfo	*sp;
	for ( i = 0; i < 2; ++i )
	{
		sp = &speedInfo[i];	
		sp->currentSpeed = NORMAL_SPEED;
		sp->speedGoal = NORMAL_SPEED;
		sp->speedInterval = DEFAULT_SPEED_INTERVAL;
	}
}	

UInt8
speedMatch(UInt8 side)
{
	return speedInfo[side].speedGoal == speedInfo[side].currentSpeed;
}
	
void 
checkSpeed()
{
	UInt8 i;
	bank1 SpeedInfo	*sp;
	for ( i = 0; i < 2; ++i )
	{
		sp = &speedInfo[i];
		if ( !timerReady(speedTimers[i]) )
			continue;
		setTimer(speedTimers[i],sp->speedInterval );
		if ( sp->currentSpeed < sp->speedGoal )
			++(sp->currentSpeed);
		else if ( sp->currentSpeed > sp->speedGoal )
			--(sp->currentSpeed);
	}
}
