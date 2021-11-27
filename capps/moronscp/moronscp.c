#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "playnote.h"

#define CLICKS	24
#define LOW_NOTE	36
#define SPREAD		6
#define NUM_NOTES	8
#define BASE_PC		7
#define PC_SPREAD	26
#define PULSE_TIMER	(NUM_NOTES)
#define PC_TIMER	(PULSE_TIMER+1)

DECLARE_QUEUES(1,16);
DECLARE_INT_FLAGS;
DECLARE_TIMERS(NUM_NOTES+2);
DECLARE_NOTES(NUM_NOTES,0);


__CONFIG(UNPROTECT & XT & PWRTEN & BORDIS & WDTEN);
bank1 UInt16	tempo;
bank1 UInt16 	pulse;
bank1 UInt8		state;

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
checkMoronscape()
{
	UInt8	i;
	UInt8	time;
	for ( i = 0; i < NUM_NOTES; ++i )
	{
		if ( timerReady(i) )
		{
			time = random(2,128);
			setTimer(i,pulse*time);
			playNote(LOW_NOTE + random(1,SPREAD) + i * SPREAD,
						random(1,128), random(1,time));
		}
	}
}

void
setPauses()
{
	UInt8 i;
	for( i = 0; i < NUM_NOTES; ++i )
	{
		setTimer(i,random(2,128) * pulse);
	}
}

enum {
	PLAYING,
	WAITING
};

void
setLEDS()
{
	UInt8 i;
	UInt8 mask = 0;
	for ( i = 0; i < NUM_NOTES; i++ )
	{
		if ( noteList[i].time )
			mask |= 1 << ((noteList[i].note - LOW_NOTE) / SPREAD);
	}
	PORTB = mask;
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
	tempo = 960;
	pulse = tempo / CLICKS;
	state = WAITING;
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
			if ( timerReady(PULSE_TIMER) )
			{
				checkNotes();
				setLEDS();
				setTimer(PULSE_TIMER,pulse);
			}
			if ( state == PLAYING )
				checkMoronscape();

			if ( timerReady(PC_TIMER) )
			{
				switch ( state )
				{
					case WAITING:
						putPgm(random(BASE_PC,BASE_PC+PC_SPREAD),0);
						setTimer(PC_TIMER,0xFA00);
						setPauses();
						state = PLAYING;
						break;

					case PLAYING:
						setTimer(PC_TIMER,5 * 2000);
						state = WAITING;
						break;
				}
			}
		}

	}
}
