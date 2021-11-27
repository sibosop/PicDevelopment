#include <pic.h>
#include <sys.h>
#include "types.h"
#include "timers.h"
#include "interrupt.h"
#include "sumovore.h"
#include "tweet.h"

TWEEK_MILLI(10,18);
DECLARE_TIMERS(MaxTimer);
DECLARE_INT_FLAGS;

#define HEARTBEAT_LED	RB5
void
init()
{
	TRISB = 0x01;	// B0 input
	OPTION = 0x8F;	// disable pullups
	TRISD = 0x64;
	TRISC = 0xE0;
	TRISA = 0xff;
	TRISE = 0x7;
}

int
main()
{
	initInterrupt(1,1);
	// initRandom();
	initTimers();
	init();
	initTweet();
	ei();

	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz ) 
		{
			oneKhz = 0;
			checkTimers();
			if ( timerReady(HeartbeatTimer) )
			{
				setTimer(HeartbeatTimer,1000);
				HEARTBEAT_LED = HEARTBEAT_LED ? 0 : 1;
			}
			checkTweet();
			if ( tweetStopped() )
				startTweet();
		}
	}
}
