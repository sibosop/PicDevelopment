#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"


DECLARE_QUEUES(1,32);
DECLARE_INT_FLAGS;
DECLARE_TIMERS(4);
typedef struct {
	UInt8	note;
	UInt8	vel;
} Note;

#define CHORD_TIMER		0
#define ROOT_TIMER		1
#define BASS_TIMER  	2


#define CLICKS	24
bank1 Note 		chord[5];
bank1 UInt16	pulse;
bank1 UInt16	tempo;
bank1 UInt8		root;
bank1 UInt8		nextRoot;
bank1 UInt8		bassPatt;
bank1 UInt8	 	pattCount;
bank1 UInt8		bassNote;
bank1 UInt16	bassTime;

enum {
	ROOTOFF,
	NEXTOFF,
	STOP
};

#define QUARTER	(CLICKS)
#define EIGHTH	(CLICKS/2)
#define HALF	(CLICKS*2)
#define SIXTEENTH (CLICKS/4)



const UInt8 pattOne[] =
{
	ROOTOFF, 36, (3*QUARTER)+(EIGHTH)+SIXTEENTH, NEXTOFF, 38, SIXTEENTH, STOP 
};

const UInt8	*bassLines[] =
{
	pattOne
};

void
playBass()
{
	putNote(bassNote,0,0);
	do {
		switch ( bassLines[bassPatt][pattCount] )
		{
			case ROOTOFF:
				++pattCount;
				bassNote = root + bassLines[bassPatt][pattCount];
				++pattCount;
				bassTime = bassLines[bassPatt][pattCount]; 
				++pattCount;
				break;

			case NEXTOFF:
				++pattCount;
				bassNote = nextRoot + bassLines[bassPatt][pattCount];
				++pattCount;
				bassTime = bassLines[bassPatt][pattCount]; 
				++pattCount;
				break;

			default:
			case STOP:
				pattCount = 0;
				break;
		}
	} while (!pattCount);
	putNote(bassNote,65,0);
	setTimer(BASS_TIMER,bassTime * pulse);

}



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
void
playChord()
{
	UInt8 i;
	for ( i = 0; i < 5; ++i )
	{
		putNote(chord[i].note, chord[i].vel, 0);
	}
}

const UInt8 triOffsets[] = {10,14,16};
void
playTriChord(UInt8 root)
{
	chord[0].note = root;
	chord[0].vel  = random(30,50);
	chord[1].note = root + 6;
	chord[1].vel  = random(30,50);
	chord[2].note = root + triOffsets[random(0,sizeof(triOffsets))];
	chord[2].vel  = random(35,55);
	chord[3].note = chord[2].note + 4;
	chord[3].vel  = random(35,55);
	chord[4].note = chord[3].note + 3;
	chord[4].vel  = random(35,55);
	playChord();
}

void
play7Chord(UInt8 root)
{
	chord[0].note = root;
	chord[0].vel  = random(30,50);
	chord[1].note = root + 10;
	chord[1].vel  = random(30,50);
	chord[2].note = chord[0].note + 14;
	chord[2].vel  = random(35,55);
	chord[3].note = chord[2].note + random(3,5);
	chord[3].vel  = random(35,55);
	chord[4].note = chord[0].note + 21;
	chord[4].vel  = random(35,55);
	playChord();
}

void
stopChord()
{
	UInt8 i;
	for ( i = 0; i < 5; ++i )
	{
		chord[i].vel = 0;
	}
	playChord();
}


void
main()
{
	UInt8 velFlag = 0;
	

	init();
	initInterrupt(0,1);
	initSerial(0,1);
	initTimers();
	initRandom();
	ei();	// enable interrupts
	tempo = 1920;
	pulse = tempo / CLICKS;
	nextRoot = random(0,12);
	bassPatt = 0;
	pattCount = 0;
	setTimer(CHORD_TIMER,pulse * EIGHTH);
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
			if ( timerReady(ROOT_TIMER) )
			{
				root = nextRoot;
				nextRoot = random(0,12);
				setTimer(ROOT_TIMER,pulse * (CLICKS * 4));
			}

			if ( timerReady(BASS_TIMER) )
			{
				playBass();		
			}
#if 1
			if ( timerReady(CHORD_TIMER) )
			{
				RC5 = 1;
				setTimer(CHORD_TIMER,(pulse * SIXTEENTH) * random(1,7));
				stopChord();
				switch ( random(0,3) )
				{
					default:
						play7Chord(root + 48);	
						break;
					case 0:
						playTriChord(root + 48 + 4);
						break;
					case 1:
						play7Chord(root + 48);	
						break;
				}
			}
#endif

		}
	}
}
