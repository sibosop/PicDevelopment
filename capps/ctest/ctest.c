#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "a2d.h"

DECLARE_INT_FLAGS;

void
init()
{
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0xFF;	// B is all inputs
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
}

#if 0
#define NUM_NOTES  8

UInt8	noteVel[NUM_NOTES];

#define NOTE_BASE	60
void
initNotes()
{
	UInt8 i;
	for ( i = 0; i < sizeof(noteVel); ++i )
	{
		noteVel[i] = 0;
	}
}

void
checkNotes()
{
	UInt8 i;
	for ( i = 0; i < NUM_NOTES; ++i )
	{
		if (timerReady(i))
		{
			RC5 = 1;
			if ( noteVel[i] )
			{
				noteVel[i] = 0;
			}
			else
			{
				noteVel[i] = 100;
			}
			putNote(i+NOTE_BASE,noteVel[i],0);	
			if ( i == 0 )
			{
				setTimer(i,1000);
			}
			else
			{
				setTimer(i,random(100,2000));
			}
		}
	}
}
#else
#define NUM_CTRLS	5
#define CTRL_BASE	4
UInt8	ctrlVals[NUM_CTRLS];
void 
initCtrl()
{
	UInt8 i;
	for(i = 0; i < NUM_CTRLS; ++i )
	{
		ctrlVals[i] = 0xff;
	}
	setTimer(0,30);
}

void
checkCtrl()
{
	if ( timerReady(0) )
	{
		UInt8 i;
		UInt8 newVal;
		for ( i = 0; i < NUM_CTRLS; ++i )
		{
			newVal = getA2D(i);	
			newVal >>= 1;
			if ( newVal != ctrlVals[i] )
			{
				RC5 = 1;
				ctrlVals[i] = newVal;
				putCtrl(i+CTRL_BASE,newVal,0);
			}
		}
		setTimer(0,30);
	}
}
#endif


void
main()
{

	init();
	initInterrupt(0,1);
	initSerial(0,1);
	initTimers();
	initRandom();
	initA2D();
#if 0
	initNotes();
#else
	initCtrl();
#endif
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC4 = 0;
		RC5 = 0;
		checkA2D();
		if ( oneKhz )
		{
			oneKhz = 0;
			RC4 = 1;
			checkTimers();
#if 0
			checkNotes();
#else
			checkCtrl();
#endif
		}
	}
}
