#include <pic.h>
#include <sys.h>
#include "types.h"
#include "timers.h"
#include "interrupt.h"

//__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN);

TWEEK_MILLI(10,18);

extern void doStop();

DECLARE_TIMERS(4);

DECLARE_INT_FLAGS;
Boolean	pudWait;
Boolean running;
Boolean msgLookup;
Boolean pudReset;
Boolean countMsg;
Boolean initFlag;
Boolean randomMsg;
//Boolean sleepFlag;
//Boolean isSleeping;

UInt8 msgNum;
UInt8 curMsg;
UInt8 maxMsg;
UInt8 loopCnt;

#define EOM_LOW				RB0
#define POWER_DOWN_HIGH		RB1
#define CHIP_ENABLE_LOW		RB2
#define ADDR0				RB3

#define HEARTBEAT_LED		RD0
#define POWER_DOWN_HIGH_LED	RD1
#define CHIP_ENABLE_LOW_LED	RD2
#define ADDR0_LED			RD3
#define PIN_INT_LED			RD4


//#define 	PUD_WAIT_TIME		28
#define		PUD_WAIT_TIME		50
#define		FIRST_EOM_WAIT_TIME	200
#define 	EOM_WAIT_TIME		100

#define PUD_TIMER  0
#define EOM_TIMER  1
#define HEARTBEAT	2
#define WAIT_MSG	3

void
init()
{
	TRISB = 0x01;	// B0 input
	OPTION = 0x8F;	// disable pullups
	TRISD = 0;
	TRISC = 0;
	TRISA = 0;
}


void
initVoc()
{
	initFlag = 1;	// start in init state
	running = 1;	// start in running state
	pinInt = 0;		// end of message flag not set
	pudWait = 0;	// not in power up wait
	msgLookup = 0;	// not in message lookup
	pudReset = 0;	// not in power up reset
	countMsg = 0;	// not counting messages
	randomMsg = 0;	// not playing random messages
//	sleepFlag = 0;
//	isSleeping = 0;
	loopCnt = 0;	// number of time to play message
	CHIP_ENABLE_LOW = 1; // disable chip
	ADDR0 = 0;			
	POWER_DOWN_HIGH = 1; 	// put in power down state
	POWER_DOWN_HIGH_LED = 1;
	INTEDG = 0;
}

// set a pulse to the enable bit
#define PULSE_ENABLE	\
	CHIP_ENABLE_LOW = 0;	\
	CHIP_ENABLE_LOW = 1;


// 
// set up the message number for the next round
// after the current msg stops
void
setupNextMsg()
{
	// number of times to play message
	loopCnt = 4;	// XXX - should be random

	// choose which message to play
	if ( ++msgNum >= maxMsg )	// xxx - should be random
		msgNum = 0;

}


// set the powerup wait state
void
doPudWait()
{
	setTimer(PUD_TIMER, PUD_WAIT_TIME );
	pudWait = 1;
}

// play next message, either silently, if we are counting
// or out loud if we are at the right message
void
playNext()
{
	// are we counting the number of messages
	if ( countMsg )
	{
		// set end of message wait timer
		setTimer(EOM_TIMER, EOM_WAIT_TIME );
		++maxMsg;	// bump the number of messages found
		ADDR0 = 1;	// put in fast lookup mode
		ADDR0_LED = 1;
	}
	else
	{
		// are we at the chosen message?
		if ( curMsg == msgNum )
		{
			// yes, have we played it enough times
			if ( !--loopCnt )
			{
				// set new message to play next time
				setupNextMsg();
				running = 0;
				doStop();
				return;
			}
			else
			{
				// we're not looking
				msgLookup = 0;
				ADDR0 = 0;	// put in play mode
				ADDR0_LED = 0;
			}
		}
		else
		{
			// we're not at the chosen message
			// bump the count and set to silent mode
			++curMsg;
			ADDR0 = 1;
			ADDR0_LED = 1;
	
		}
	}
	PULSE_ENABLE;	// pulse enable to play next message
}

// polling routine that checks the start of the
// power up pin
void
checkPud()
{
	// if we ar  initializing then don't do this
	if ( initFlag ) return;

	// if are we waiting for a timer?
	if ( !timerReady(PUD_TIMER) ) return;
	// no longer waiting
	pudWait = 0;
	// if we were reseting, then finish it
	if ( pudReset )
	{
		// clear the reset mode
		pudReset = 0;

		// if we've been turned off in the mean
		// time, then just leave it
		if ( !running ) return;

		
		// power up the chip again
		POWER_DOWN_HIGH_LED = 0;
		POWER_DOWN_HIGH = 0;
		// set the timer of the power bit
		doPudWait();
	}
	else
	{
		// okay, power bit has been
		// pulsed, play the next note
		playNext();
	}
}

//
// start the reset pulse.
//
void
doPudReset()
{
	// power down the chip
	POWER_DOWN_HIGH = 1;
	POWER_DOWN_HIGH_LED = 1;
	// say that we are on the
	// first part of the power cycle
	pudReset = 1;
	curMsg = 0;		// back to message 0
	msgLookup = 1;	// back in message lookup mode
	doPudWait();	// set the pud timer
}


// stop the chip
void
doStop()
{
	ADDR0 = 0;	// normal play mode
	ADDR0_LED = 0;
	CHIP_ENABLE_LOW = 1;	// no record
	CHIP_ENABLE_LOW_LED = 1;
	POWER_DOWN_HIGH = 1;	// power off
	POWER_DOWN_HIGH_LED = 1; // power off
	curMsg = 0;				// reset message count
	doPudReset();			// cycle the power up pin
}

// this gets called every killosecond
void
checkVoc()
{
	if ( initFlag )
	{
		// are we just starting up
		maxMsg = 0;		// clear the number of messages in chip
		initFlag = 0;	// say we're done initializing
		countMsg = 1;	// say that we're ready to count messages
		setTimer(EOM_TIMER,FIRST_EOM_WAIT_TIME);	// wait a bit
		doStop();		// ture the chip off
	}
	else if ( countMsg )

	{
		// EOM doesn't happen on last message
		// so we time out waiting and get
		// here. The EOM_TIMER gets
		// set every time we start a new
		// message in silent mode
		if ( timerReady( EOM_TIMER ) )
		{
			countMsg = 0;	// we are no longer counting
			if ( !maxMsg )
			{
				// either, the chip is empty or something
				// bad happened
				while (1);	// cause software reset
			}
			setTimer(WAIT_MSG,2000);
			--maxMsg;			// last message wasn't there
			randomMsg = 1;		// we are in random mode always
			loopCnt = 1;		// set loop to expire
			setupNextMsg();		// set the next message before starting
		//	sleepFlag = 0;		// don't ever sleep on the first time
			doStop();			// this stops the message counting
		}
		return;
	}
	else
	{
		//doPudReset();
	}
}




int
main()
{
	initInterrupt(1,1);
	// initRandom();
	initTimers();
	init();
	initVoc();
	ei();
	// startup wait loop
	curMsg = 1;
	while ( 1 )
	{
		CLRWDT();
		//if ( !initFlag ) checkA2D();		// get new a2d values

		// are we pulsing the power up pin
		if ( pudWait ) checkPud();

		// did we get an eom interrupt?
		if ( pinInt )
	    {
			PIN_INT_LED = PIN_INT_LED ? 0 : 1;
			pinInt = 0;
			if ( !initFlag ) 
			{
				// if we are in msg mode then
				// play next message (or current
				// message)
				// if not, then we need to reset
				// the chip to look for the next
				// message
				if ( msgLookup )
				{
					playNext();
				}
				else
				{
#if 0
					if ( sleepFlag )
					{
						sleepFlag = 0;
						isSleeping = 1;
						setTimer(WAIT_MSG, 10000);
						doStop();
	//					CLRWDT();
	//					SLEEP();
					}
					else
#endif
					doPudReset();
				}
			}
		}
			
		if ( oneKhz ) 
		{
			oneKhz = 0;
			checkTimers();
			if ( timerReady(HEARTBEAT) )
			{
				setTimer(HEARTBEAT,1000);
				HEARTBEAT_LED = HEARTBEAT_LED ? 0 : 1;
			}
			if ( !running )
			{
				if ( timerReady(WAIT_MSG) )
				{
					running = 1;
					doPudReset();
				}
			}
			checkVoc();
		}
	}
}

