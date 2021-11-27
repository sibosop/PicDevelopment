#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "playnote.h"


DECLARE_QUEUES(1,16);
DECLARE_INT_FLAGS;
DECLARE_TIMERS(4);
DECLARE_NOTES(12);


#define PULSE_TIMER	0
#define CHORD_TIMER 1
#define ROOT_TIMER	2

#define HARMONY_TIMER	ROOT_TIMER
#define MELODY_TIMER	3

#define CLICKS	24

bank1 UInt8		root;
bank1 UInt8		nextRoot;
bank1 UInt16	tempo;
bank1 UInt16 	pulse;
bank1 UInt8		measureCount;
bank1 UInt8		songIndex1;
bank1 UInt8		songLine1;
bank1 UInt8		songNum;
bank1 UInt8		harmony[3];

#define chordIndex	root	// for Phil


enum {
	Phil,
	Jazz
};


#define WHOLE	(CLICKS * 4)
#define HALF	(CLICKS * 2)
#define QUARTER	CLICKS
#define EIGHTH	(CLICKS/2)
#define SIXTEENTH	(CLICKS/4)



void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all output
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port

}

const UInt8 triOffsets[] = {10,14,16};


#define CHORD_VOL_LOW	0
#define CHORD_VOL_HIGH	50
	
void
playTriChord(UInt8 root, UInt8 t)
{
	UInt8 calNote;
	playNote(root, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	playNote(root+6, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	
	calNote = root+triOffsets[random(0,sizeof(triOffsets))];
	playNote(calNote, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	calNote += 4;
	playNote(calNote, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	calNote += 3;
	playNote(calNote, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
}

void
play7Chord(UInt8 root, UInt8 t)
{
	UInt8	calNote = root + 14;
	playNote(root, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	playNote(root+10, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	playNote(calNote, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	calNote += random(3,5);
	playNote(calNote, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
	playNote(root+21, random(CHORD_VOL_LOW,CHORD_VOL_HIGH), t);
}

enum {
	OFF_ROOT,
	OFF_NEXTROOT,
};

#define STOP 0xFF

const UInt8 bassLine1[] =
{
	0,               OFF_ROOT    , 36, QUARTER,
	QUARTER-SIXTEENTH, OFF_ROOT	 , 43, EIGHTH,
	(QUARTER*3)-SIXTEENTH,OFF_ROOT, 43, EIGHTH,
	(QUARTER*4)-SIXTEENTH,OFF_NEXTROOT, 37, SIXTEENTH,
	STOP
};

const UInt8 bassLine2[] =
{
	0,               OFF_ROOT    , 36, EIGHTH,
    EIGHTH,          OFF_ROOT    , 24, EIGHTH,
	QUARTER-SIXTEENTH, OFF_ROOT	 , 43, EIGHTH,
	(QUARTER*3)-SIXTEENTH,OFF_ROOT, 43, EIGHTH,
	(QUARTER*4)-(SIXTEENTH*2),OFF_NEXTROOT, 38, SIXTEENTH,
	(QUARTER*4)-SIXTEENTH,OFF_NEXTROOT, 37, SIXTEENTH,
	STOP
};

const UInt8 bassLine3[] =
{
	0,                  OFF_ROOT    , 36, SIXTEENTH,
    SIXTEENTH,          OFF_ROOT    , 43, SIXTEENTH,
    SIXTEENTH*2,        OFF_ROOT    , 42, SIXTEENTH,
    SIXTEENTH*3,        OFF_ROOT    , 36, SIXTEENTH,
	QUARTER-SIXTEENTH, OFF_ROOT	 , 24, EIGHTH,
	(QUARTER*3)-SIXTEENTH,OFF_ROOT, 43, EIGHTH,
	(QUARTER*4)-(SIXTEENTH*2),OFF_NEXTROOT, 38, SIXTEENTH,
	(QUARTER*4)-SIXTEENTH,OFF_NEXTROOT, 37, SIXTEENTH,
	STOP
};

const UInt8 *bassLines[] =
{
	bassLine1,
	bassLine2,
	bassLine3
};

UInt8
checkPhrase(const UInt8 *pl, UInt8 index)
{
	UInt8 	n = 0xff;
	if ( pl[index] == measureCount )
	{
		switch ( pl[++index] )
		{
			default:
				index = 0;
				break;

			case OFF_ROOT:
			    n = root + pl[++index];;
				break;

			case OFF_NEXTROOT:
			    n = nextRoot + pl[++index];;
				break;
		}
		if ( n != (UInt8)0xFF)
		{
			playNote(n,random(35,55),pl[++index]);
			++index;
		}
	}
	if ( pl[index]  == STOP )
		index = 0;
	return index;

}

void
initJazz(void)
{
	tempo = 1920;
	nextRoot = random(0,12);
	measureCount = 0;
}

void
doJazz(void)
{
	UInt8	chordTime;

	if ( timerReady (ROOT_TIMER) )
	{
		root = nextRoot;
		nextRoot = random(0,12);
		setTimer(ROOT_TIMER, pulse * QUARTER * 4);
	}

	if ( timerReady(PULSE_TIMER) )
	{
		setTimer(PULSE_TIMER,pulse);
		checkNotes();
		songIndex1 = checkPhrase(bassLines[songLine1],songIndex1);
		if ( songIndex1 == 0 )
			songLine1 = random(0,3);
		if ( ++measureCount == (QUARTER * 4) )
			measureCount = 0;
	}

	if ( timerReady(CHORD_TIMER) )
	{
		RC5 = 1;
		chordTime = SIXTEENTH * random(1,4);
		setTimer(CHORD_TIMER,pulse * chordTime);
		if ( chordTime > SIXTEENTH )
			chordTime -= random(0,chordTime-SIXTEENTH);
		switch ( random(0,3) )
		{
			default:
				play7Chord(root + 48,chordTime);	
				break;
			case 0:
				playTriChord(root + 48 + 4,chordTime);
				break;
			case 1:
				play7Chord(root + 48,chordTime);	
				break;
		}
	}
}

void
initPhil()
{
	harmony[0] = 48;
	harmony[1] = 52;
	harmony[2] = 56;
	chordIndex = 0;
	tempo = 480; 
	setTimer(MELODY_TIMER, WHOLE * pulse );
}

void
changeHarmony()
{
	UInt8 index = random(0,3);
	harmony[index] -= 2;
	harmony[index] += random(0,5);

	if ( harmony[2] <= harmony[1] )
		harmony[2] = harmony[1] + random(1,3);

	if ( harmony[1] <= harmony[0] )
		harmony[0] = harmony[0] - random(1,3);
}

void
doPhil(void)
{
	UInt8 tmp;

	if ( timerReady(PULSE_TIMER) )
	{
		setTimer(PULSE_TIMER,pulse);
		checkNotes();
	}

	if ( timerReady( HARMONY_TIMER ) )
	{
		setTimer(HARMONY_TIMER,	pulse * QUARTER  * random(8,16));
		changeHarmony();
	}

	if ( timerReady( CHORD_TIMER ) )
	{
		setTimer(CHORD_TIMER, pulse * SIXTEENTH);
		switch(chordIndex)
		{
			case 0:
				tmp = harmony[0];
				break;

			case 1:
			case 3:
				tmp = harmony[2];
				break;

			case 2:	
				tmp = harmony[1];
				break;
		}
		playNote(tmp,random(35,50),random(SIXTEENTH,QUARTER));
		if ( ++chordIndex == 4 ) 
			chordIndex = 0;
	}
	if ( timerReady(MELODY_TIMER) )
	{
		if ( random(0,8) )
		{
			tmp = EIGHTH * random(1,4);
			playNote(harmony[random(0,3)] + (12 * random(1,3)),random(60,100),
					tmp +  random(SIXTEENTH,EIGHTH));
			setTimer(MELODY_TIMER,tmp * pulse);
		}
		else
		{
			playNote(harmony[0]-12,60,WHOLE);
			playNote(harmony[0]-24,60,WHOLE);
			playNote(harmony[0]+12,60,WHOLE);
			setTimer(MELODY_TIMER,WHOLE * 4 * pulse);
		}
	}
}

void
main()
{
	init();
	initInterrupt(0,1);
	initSerial(0,1);
	initTimers();
	initRandom();
	ei();	// enable interrupts
	songIndex1 = 0;
	songLine1 = 1;
	songNum = Phil;
	initPhil();
	pulse = tempo / CLICKS;
	while ( 1 )
	{
		CLRWDT();
		RC4 = 0;
		RC5 = 0;
		if ( oneKhz )
		{
			RC4 = 1;
			oneKhz = 0;
			checkTimers();
			switch(songNum)
			{
				case Phil:
					doPhil();
					break;

				case Jazz:
					doJazz();
					break;

				default:
					break;
			}

		}

	}
}
