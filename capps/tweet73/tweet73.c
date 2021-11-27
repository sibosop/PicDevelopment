#include <pic.h>
#include <sys.h>
#include "types.h"
#include "random.h"
#include "timers.h"
#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "tweet73.h"

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & BORDIS);

DECLARE_TIMERS(2);
DECLARE_INT_FLAGS;

Boolean	pudWait;
Boolean running;
Boolean msgLookup;
Boolean pudReset;
Boolean countMsg;
Boolean initFlag;
Boolean randomMsg;
Boolean sleepFlag;
Boolean	recording;
Boolean paused;

UInt8 msgNum;
UInt8 curMsg;
UInt8 maxMsg;
UInt8 loopCnt;
UInt8 bankNum;

#define RECORD_ENABLE_LOW_LED RA3
#define POWER_DOWN_HIGH_LED	RA2
#define CHIP_ENABLE_LOW_LED	RA1
#define ADDR0_LED			RA0

#define EOM_LOW				RB0
#define POWER_DOWN_HIGH		RB1
#define RECORD_ENABLE_LOW	RB2
#define CHIP_ENABLE_LOW		RB3
#define ADDR0				RB4
#define OVERFLOW			RB5

#define 	PUD_WAIT_TIME		28
#define		FIRST_EOM_WAIT_TIME	200
#define 	EOM_WAIT_TIME		100

#define PUD_TIMER  		0
#define EOM_TIMER		1

void
init()
{
	ADCON1 = 6;		// make port a all digital not analog
	TRISA = 0x0;	// A0,A1,A4 inputs A2,A3 outputs
	TRISB = 0x1;	// B0 input, all else outputs
	OPTION = 0x8F;	// disable pullups
	PORTB = 0;
}


void
initVoc()
{
	loopCnt   = 1;	// number of time to play message
	pudWait   = 0;	// not in power up wait
	running   = 0;	// start in running state
	msgLookup = 0;	// not in message lookup
	countMsg  = 0;	// not counting messages
	initFlag  = 1;	// start in init state
	randomMsg = 0;	// not playing random messages
	sleepFlag = 0;
	recording = 0;
	paused    = 0;
	pinInt    = 0;		// end of message flag not set
	pudReset  = 0;	// not in power up reset
	bankNum   = 0;
	msgNum    = 4;

	RECORD_ENABLE_LOW = 1;
	RECORD_ENABLE_LOW_LED = 1;
	CHIP_ENABLE_LOW = 1; // disable chip
	CHIP_ENABLE_LOW_LED = 1;
	ADDR0 = 0;			
	ADDR0_LED = 0;
	POWER_DOWN_HIGH = 1; 	// put in power down state
	POWER_DOWN_HIGH_LED = 1;
	
}

// set a pulse to the enable bit
#define PULSE_ENABLE	\
	CHIP_ENABLE_LOW = 0;	\
	CHIP_ENABLE_LOW_LED = 0; \
	CHIP_ENABLE_LOW = 1;	\
	CHIP_ENABLE_LOW_LED = 1

// 
// set up the message number for the next round
// after the current msg stops
void
setupNextMsg()
{
	if ( randomMsg )
	{
		if ( random(0,10) == 5 )
			sleepFlag = 1;
		// number of times to play message
		loopCnt = (UInt8)random(1,4); 

		// choose which message to play
		msgNum  = (UInt8)random(0,maxMsg);
	}
	else
	{
		loopCnt = 1;
	}
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
			}
			// we're not looking
			msgLookup = 0;
			ADDR0 = 0;	// put in play mode
			ADDR0_LED = 0;
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

		if ( recording )
		{
			RECORD_ENABLE_LOW = 0;
			RECORD_ENABLE_LOW_LED = 0;
			ADDR0 = 0;
			ADDR0_LED = 0;
		}

		// if we've been turned off in the mean
		// time, then just leave it
		if ( !running ) return;

		
		// power up the chip again
		POWER_DOWN_HIGH = 0;
		POWER_DOWN_HIGH_LED = 0;
		// set the timer of the power bit
		doPudWait();
	}
	else
	{
		if ( !running ) return;

		if ( recording )
		{
			CHIP_ENABLE_LOW = 0;
			CHIP_ENABLE_LOW_LED = 0;
		}
		else
		{
			// okay, power bit has been
			// pulsed, play the next note
			playNext();
		}
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
		doPlay();		// ture the chip off
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
			if ( maxMsg )
			{
				--maxMsg;			// last message wasn't there
			}
			randomMsg = 0;		// we are in random mode always
			loopCnt = 1;		// set loop to expire
			setupNextMsg();		// set the next message before starting
			sleepFlag = 0;		// don't ever sleep on the first time
			doStop();			// this stops the message counting
		}
		return;
	}
}


void
handleOverflow()
{
	recording = 0;
	paused = 0;
	running = 0;
	doStop();
}



main()
{
	initInterrupt(1,1);
	initRandom();
	initSerial(1,0);
	initTimers();
	init();
	initVoc();
	initmparse();
	ei();
	// startup wait loop

	while ( 1 )
	{
		CLRWDT();
		checkmparse();

		// are we pulsing the power up pin
		if ( pudWait ) checkPud();

		// did we get an eom interrupt?
		if ( pinInt )
	    {
			pinInt = 0;
			if ( !initFlag ) 
			{
				if ( !running ) continue;
				if ( paused ) continue;

				if ( recording )
				{
					handleOverflow();
				}

				// if we are in msg mode then
				// play next message (or current
				// message)
				// if not, then we need to reset
				// the chip to look for the next
				// message
				else if ( msgLookup )
				{
					playNext();
				}
				else
				{
#if 1
					if ( sleepFlag )
					{
						UInt8 sleepLoop;
						RB7 = 1;
						asm("nop");
						asm("nop");
						asm("nop");
						asm("nop");
						asm("nop");
						RB7 = 0;
						sleepFlag = 0;
						doStop();
						sleepLoop = random(1,5);
						while ( sleepLoop-- )
						{
							CLRWDT();
							SLEEP();
						}
						asm("nop");
						asm("nop");
						RB7 = 1;
						asm("nop");
						asm("nop");
						asm("nop");
						asm("nop");
						asm("nop");
						doPlay();
					}
					else
					{
						doPudReset();
					}
#endif
				}
			}
		}
			
		if ( oneKhz ) 
		{
			RB6 = RB6 ? 0 : 1;
			oneKhz = 0;
			checkTimers();
			checkVoc();
		}
	}
}

//
// Control change message controls the voice chip
//		0 - play
//		1 - record
//		2 - stop
//		3 - pause
//		4 - resume
void
handleControlChange()
{
	if ( mbyte1 != 4 )
		return;  // not the right control number

	if ( mbyte2 == 0 )
	{
		doPlay();
	} 
	else if ( mbyte2 == 1 )
	{
		doRecord();
	}
	else if ( mbyte2 == 2 )
	{
		doStop();
	}
	else if ( mbyte2 == 3 )
	{
		doPause();
	}
	else if ( mbyte2 == 4 )
	{
		doResume();
	}
	else if ( mbyte2 == 5 )
	{
		doBankChange(0);
	}
	else if ( mbyte2 == 6 )
	{
		doBankChange(1);
	}
	else if ( mbyte2 == 7 )
	{
		randomMsg = 1;
	}
}

void
doPlay()
{
	if ( running ) 
		return;

	if ( paused )
		return;

	running = 1;
	recording = 0;
	curMsg = 0;
	doPudReset();
}

void
doRecord()
{
	if ( running ) 
		return;

	if ( paused )
		return;

	running = 1;
	recording = 1;
	maxMsg = 0;
	doPudReset();
}


// stop the chip
void
doStop()
{
	running = 0;
	paused = 0;
	if ( recording )
	{
		recording = 0;
		initFlag = 1;
	}
	ADDR0 = 0;	// normal play mode
	ADDR0_LED = 0;
	CHIP_ENABLE_LOW = 1;	// no record
	CHIP_ENABLE_LOW_LED = 1;
	POWER_DOWN_HIGH = 1;	// power off
	POWER_DOWN_HIGH_LED = 1; // power off
	RECORD_ENABLE_LOW = 1;
	RECORD_ENABLE_LOW_LED = 1;
	curMsg = 0;	
}

void
doPause()
{
	if ( !running )
		return;

	if ( paused )
		return;

	paused = 1;
	if ( !recording )
		return;

	CHIP_ENABLE_LOW = 1;
	CHIP_ENABLE_LOW_LED = 1;
}

void 
doResume()
{
	if ( !running )
		return;
	if ( !paused )
		return;

	paused = 0;
	if ( !recording )
		doPlay();
	else
	{
		CHIP_ENABLE_LOW = 0;
		CHIP_ENABLE_LOW_LED = 0;
	}
}

void
doBankChange(UInt8 newBank)
{
	bankNum = newBank;
}

void
handleProgramChange()
{
	UInt8 msg;
	ADDR0_LED = 0;
	randomMsg = 0;
	msg = mbyte1 + (bankNum  * 128);
	if ( mbyte1 >= maxMsg )
	{
		msgNum = 0;
	}
	else
	{
		msgNum = msg;
	}
}

void
msgErr()
{
	ADDR0_LED = 0;
	msgNum = 0;
}

