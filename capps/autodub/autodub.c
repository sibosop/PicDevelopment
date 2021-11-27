#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "playnote.h"

DECLARE_TIMERS(4);
DECLARE_INT_FLAGS;

#if defined(_16F876)
#include "serial.h"
#endif


DECLARE_QUEUES(16,48);

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);






#define NUM_DRUM_VOICES 16
#define ACCENT_STRONG	127
#define ACCENT_WEAK	80
#define NO_ACCENT	60
#define DRUM_OFFSET	60
#define DRUM_CHANNEL	0
#define	DRUM_TIMER	0
#define LAST_DRUM_TIMER	1

#define MIX_CHANNEL	1
#define MIXER_TIMER	2
#define MAX_MIX_ON	50
#define MIN_MIX_ON	6
#define MUTE_ON		127
#define	MUTE_OFF	1
#define NUM_MUTES	8
#define MUTE_LOW	37

#define SWELL_TIMER	3


UInt16 drumMask[NUM_DRUM_VOICES];
UInt8  drumCnts[NUM_DRUM_VOICES];
bank3 UInt8  accentStrong[NUM_DRUM_VOICES];
bank3 UInt8  accentWeak[NUM_DRUM_VOICES];
bank3 UInt8  drumMax[NUM_DRUM_VOICES];


UInt8 running;
UInt8 count;
UInt8 muteCount;
UInt8 clockFlag;
UInt8	lastDrum;
bank2 UInt8	swellCount;
bank2 UInt8	swellDrum;
bank2 UInt8	swellInterval;

bank2 UInt8 muteMask[NUM_MUTES];
bank3 UInt8  muteCnts[NUM_MUTES];
bank3 UInt8  muteMax[NUM_MUTES];




DECLARE_NOTES(16,MIX_CHANNEL);

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;


}

void
lightOn(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB |= 1 << m;
	}
	else
	{
		PORTC |= 0x10 << (m-6);
	}
}

void
lightOff(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB &= ~(1<<m);
	}
	else
	{
		PORTC &= ~(0x10<<(m-6));
	}
}

void
muteOff(UInt8 n, UInt8 t)
{
	UInt8 i;
	for( i = 0; i < numNotes; i++ )
	{
		if (!noteList[i].time)
		{
			lightOn(n - MUTE_LOW );
			putNote(n,MUTE_OFF,pnChannel);
			noteList[i].note = n;
			noteList[i].time = t;
			break;
		}
	}
}

void
checkMute(void)
{
	UInt8 i;
	for( i = 0; i < numNotes; i++ )
	{
		if (noteList[i].time)
		{
			if ( --noteList[i].time == 0 )
			{
				putNote(noteList[i].note,MUTE_ON,pnChannel);
				lightOff(noteList[i].note - MUTE_LOW );
			}
		}
	}
}

void
initMixer()
{
	UInt8 i;
	putNote(MUTE_LOW-1,MUTE_OFF, pnChannel);
	for ( i = 0; i < NUM_MUTES; ++i)
		putNote(MUTE_LOW+i,MUTE_ON,pnChannel);


}

void
checkMixer()
{
	UInt8 	i;
	UInt16	mask;

	for(i = 0; i < NUM_MUTES; i++ )
	{
		mask = 1 << muteCnts[i];
		if ( mask & muteMask[i] )
		{
			muteOff(MUTE_LOW + i,random(8,15));	
		}
		++muteCnts[i];
		if ( muteCnts[i] >= muteMax[i] )
			muteCnts[i] = 0;
	} 
}

void
initMute(UInt8 i)
{
	UInt8	j;
	UInt8	max;
	UInt8	tmp;
	UInt8 	cnt;

	CLRWDT();
	muteCnts[i] = 0;
	muteMask[i] = 0;
	max = muteMax[i] = random(3,8+1);
	if ( !max ) return;

	//cnt = random(0,max > 2 ? 2 : max);
	cnt = random(0,3);
	for (j = 0; j < cnt; ++j )
	{
		tmp = random(0,max);
		muteMask[i] |= 1 << tmp;
	}
}
void
resetMutes()
{
	UInt8 i;
	for ( i = 0; i < NUM_MUTES; ++i)
	{
		muteMask[i] = 0;
		muteCnts[i] = 0;
	}
}

void 
initDrums(UInt8 i, UInt8 force)
{
	UInt8	j;
	UInt8	max;
	UInt8	tmp;
	UInt8 	cnt;
	do
	{	
		CLRWDT();
		accentStrong[i] = 0xff;
		accentWeak[i] = 0xff;
		drumCnts[i] = 0;
		drumMask[i] = 0;
		max = drumMax[i] = random(3,16+1);
		if ( !max ) return;
		if ( i == 0 )
		{
			drumMask[0] |= 1;
			cnt = random(0,3);
		}
		else
		{
			cnt = random(0,max > 4 ? 4 : max);
		}

		for (j = 0; j < cnt; ++j )
		{
			tmp = random(0,max);
			if ( accentStrong[i] == 0xff )
				accentStrong[i] = tmp;
			else if ( accentWeak[i] = 0xff )
				accentWeak[i] = tmp;
			drumMask[i] |= 1 << tmp;
		}
		if ( force )
			force = drumMask[i] ? 0 : 1;
	} while ( force );
}

void
resetDrums()
{
	UInt8 i;
	for ( i = 0; i < NUM_DRUM_VOICES; ++i)
	{
		drumMask[i] = 0;
		drumCnts[i] = 0;
	}
#if 1
	lastDrum = 0xff;
#endif
	running = 0;
}

void 
handleRealTimeStart (void)
{
	di();
	resetDrums();
	resetMutes();
	initDrums(0,1);
	running = 1;
	count = random(100,300);
	muteCount = random(100,300);
	ei();
}

void 
handleRealTimeStop (void)
{
	running = 0;
	//initMixer();
}

void
handleRealTimeContinue(void)
{
	running = 1;
}

void 
handleRealTimeClock (void)
{
	clockFlag = 1;
}


void 
handleNoteOn (void)
{
	UInt8 drum;
	if ( !mbyte2 )
		return;

	di();
	if ( (mbyte1 >= 60) && (mbyte1 < 76 ) )
	{
		drum = mbyte1 - 60;
		if ( drum == lastDrum )
		{
			drumMask[drum] = 0;
			drumCnts[drum] = 0;
		}
		else
		{
			initDrums(drum,0);
			lastDrum = drum;
			setTimer(LAST_DRUM_TIMER, 100);
		}
	}
	ei();
}

checkDrum()
{
	UInt8	vel;
	UInt8 	i;
	UInt16	mask;

	for(i = 0; i < NUM_DRUM_VOICES; i++ )
	{
		if ( i == swellDrum )
		{
			continue;
		}
		mask = 1 << drumCnts[i];
		if ( mask & drumMask[i] )
		{
			if ( accentStrong[i] == drumCnts[i] )
				vel = ACCENT_STRONG;
			else if ( accentWeak[i] == drumCnts[i] )
				vel = ACCENT_WEAK;
			else
				vel = NO_ACCENT;
			putNote(DRUM_OFFSET+i,vel,DRUM_CHANNEL);	
		}
		++drumCnts[i];
		if ( drumCnts[i] >= drumMax[i] )
			drumCnts[i] = 0;
	} 
}

void
main()
{
	UInt8	i;
	init();
	initInterrupt(0,1);
	initSerial(1,1);
	initmparse();
	initMixer();
	initTimers();
	initRandom();
	resetDrums();
	initDrums(0,1);
	swellDrum = random(3,16);
	swellInterval = random(1,5);
	ei();	// enable interrupts
	//midiReset();
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			//RC4 = RC4 ? 0 : 1;
			oneKhz = 0;
		}
		if ( clockFlag )
		{
			clockFlag = 0;
		 	checkTimers();
			checkMute();
		}
		if ( timerReady(SWELL_TIMER) )
		{
			setTimer(SWELL_TIMER,swellInterval);
			if ( running )
			{
				if ( ++swellCount == 128 )
				{
					swellCount = 0;
					swellDrum = random(3,16);
					swellInterval = random(1,5);
					setTimer(SWELL_TIMER,24 * random(4,10));
				}
				else
					putNote(DRUM_OFFSET+swellDrum, swellCount, DRUM_CHANNEL);
			}
		}
		if ( timerReady(LAST_DRUM_TIMER) )
		{
			lastDrum = 0xff;
		}

		if ( timerReady(MIXER_TIMER) )
		{
			setTimer(MIXER_TIMER,12);
			if ( running )
			{
				checkMixer();
			}
		}

		if ( timerReady(DRUM_TIMER) )
		{
			setTimer(DRUM_TIMER,6);
			if ( running )
			{
				if ( !--muteCount)
				{
					initMute(random(0,256) % NUM_MUTES);
					muteCount = random(10,120);
				}

				checkDrum();
				if (!--count)
				{
					initDrums(random(0,256) % 16,0);
					count = random(10,100);
				}
			}
		}


		if ( CharReady )
		{
			CharReady = 0;
			checkmparse();
		}
	}
}
