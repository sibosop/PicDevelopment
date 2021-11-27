#include <pic.h>
#include "types.h"
#include "timers.h"
#include "interrupt.h"
#include "tweet.h"
#include "sumovore.h"
#include "random.h"

typedef enum TwState_{
	TwNull,
	TwError,
	TwStartCount, 
	TwCountMsg,
	TwCheckLoop,
	TwFindMsg,
	TwEomWait,
	TwWaitReset,
	TwNotRunning,
	TwStart,
	TwStop,
} TwState;
	
	
TwState state;
TwState nextState;	

UInt8 msgNum;
UInt8 curMsg;
UInt8 maxMsg;
UInt8 loopCnt;

#define EOM_LOW				RB0
#define POWER_DOWN_HIGH		RB1
#define CHIP_ENABLE_LOW		RB2
#define ADDR0				RB3

#define DIAG_LED1		RB4
#define DIAG_LED2		RD1
#define	DIAG_LED3		RD0
#define POWER_DOWN_HIGH_LED	DIAG_LED1
#define CHIP_ENABLE_LOW_LED	DIAG_LED2
#define ADDR0_LED			DIAG_LED3


#define POWER_DOWN \
	POWER_DOWN_HIGH 	= 1; \
	POWER_DOWN_HIGH_LED	= 1

#define POWER_UP \
	POWER_DOWN_HIGH 	= 0; \
	POWER_DOWN_HIGH_LED	= 0

#define SET_SILENT \
	ADDR0 = 1; \
	ADDR0_LED = 1

#define SET_AUDIBLE \
	ADDR0 = 0;	\
	ADDR0_LED = 0


#define 	PUD_WAIT_TIME		28
#define		FIRST_EOM_WAIT_TIME	200
#define 	FAST_EOM_TIMEOUT		100
#define		SLOW_EOM_TIMEOUT	10000


void
startTweet()
{
	state = TwStart;
}

void
stopTweet()
{
	state = TwStop;
}

UInt8
tweetStopped()
{
	return state == TwNotRunning;
}

void
pudReset(TwState ns)
{
	POWER_DOWN;
	state = TwWaitReset;
	nextState = ns;
	setTimer(TweetTimer,PUD_WAIT_TIME);
}

void
initTweet()
{
	pinInt = 0;		// end of message flag not set
	loopCnt = 0;	// number of time to play message
	CHIP_ENABLE_LOW = 1; // disable chip
	ADDR0 = 0;			
	POWER_DOWN_HIGH = 1; 	// put in power down state
	POWER_DOWN_HIGH_LED = 1;
	INTEDG = 0;
	maxMsg = 0;		// clear the number of messages in chip
	setTimer(TweetTimer,FIRST_EOM_WAIT_TIME);	// wait a bit
	SET_SILENT;
	pudReset(TwStartCount);
}

// set a pulse to the enable bit
#define PULSE_ENABLE	\
	CHIP_ENABLE_LOW = 0;	\
	CHIP_ENABLE_LOW = 1

void
go(UInt16 timeout, TwState ns)
{
	PULSE_ENABLE;			
	state = TwEomWait;
	setTimer(TweetTimer,timeout);
	nextState = ns;
}
// 
// set up the message number for the next round
// after the current msg stops
void
setupNextMsg()
{
	// number of times to play message
	loopCnt = random(1,7);	

	// choose which message to play
	msgNum = random(0,maxMsg);

	curMsg = 0;
}



// stop the chip
void
stop()
{
	SET_AUDIBLE;
	CHIP_ENABLE_LOW = 1;
	POWER_DOWN;
	curMsg = 0;				// reset message count
}

// this gets called every killosecond
void
checkTweet()
{
	switch(state)
	{
		case TwNull:
		case TwNotRunning:
		default:
			break;

		case TwWaitReset:
			if ( timerReady(TweetTimer) )
			{
				if ( POWER_DOWN_HIGH == 1 )
				{
					POWER_UP;
					setTimer(TweetTimer,PUD_WAIT_TIME);
				}
				else
				{
					// bump of power down complete
					state = nextState;
				}
			}
			break;
		case TwStartCount:
			SET_SILENT;
			go(FAST_EOM_TIMEOUT,TwCountMsg);
			break;
			
		case TwCountMsg:
			if ( timerReady (TweetTimer) )
			{
				// EOM doesn't happen on last message
				// so we time out waiting and get
				// here. The EOM_TIMER gets
				// set every time we start a new
				// message in silent mode
	
				if ( !maxMsg )
				{
					// either, the chip is empty or something
					// bad happened
					state = TwError;	// cause software reset
				}
				--maxMsg;			// last message wasn't there
				setupNextMsg();		// set the next message before starting
				stop();			// this stops the message counting
				state = TwNotRunning;
			}
			else
			{
				++maxMsg;
				SET_SILENT;
				go(FAST_EOM_TIMEOUT,TwCountMsg);				
			}
			break;

		case TwEomWait:
			if ( pinInt == 1 )
			{
				pinInt = 0;
				state = nextState;
			}
			else if ( timerReady(TweetTimer) )
			{
				state = nextState;
			}
			break;
		
		case TwError:
			while ( 1 ) ;		// cause WD_TIMER to fire
			break;

		case TwCheckLoop:
			if ( !--loopCnt )
				setupNextMsg();
			else
			{
				curMsg = 0;
			}
			pudReset(TwFindMsg);
			break;

		case TwFindMsg:
			if ( curMsg == msgNum )
			{
				SET_AUDIBLE;
				go(SLOW_EOM_TIMEOUT,TwCheckLoop);
			}
			else
			{
				++curMsg;
				SET_SILENT;
				go(SLOW_EOM_TIMEOUT,TwFindMsg);
			}
			break;

		case TwStart:
			setupNextMsg();
			pudReset(TwFindMsg);
			break;

		case TwStop:
			stop();
			state = TwNotRunning;
			break;
	}
}



