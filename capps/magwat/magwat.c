#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "a2d.h"


bank1 const UInt8 pgms[] = {
	27,87,94,102,
	107,111,114,27,
	22,29,38,39,
	42,63,75,80
	};
Boolean pgmChangeOk;
Boolean	zero;
UInt8 destPgm;
UInt8 curPgm;	
UInt16 timeVal;

DECLARE_QUEUES(1,16);
DECLARE_A2D_VALS(2);
DECLARE_INT_FLAGS;
DECLARE_TIMERS(2);


int
abs(int a)
{
	if ( a < 0 )
		return -a;
	return a;
}

void
init()
{
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;	// B is all outputs
	
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
	curPgm = 27;
	pgmChangeOk =  1;
	zero = 1;
}

#define NUM_CTRLS	2
#define CTRL_BASE	4
#define LIGHT 0
#define LIGHT_MIN	64
//#define	FLEX_MIN	45
#define	FLEX_MIN_VAL	0
#define FLEX  1
#define PGM_TIMER	0
#define DEBOUNCE_TIMER	1
#define DEBOUNCE_TIME	3000
UInt8	ctrlVals[NUM_CTRLS];
UInt8 i;
UInt8 newVal;
UInt8 change; 

void 
initCtrl()
{
	newVal = 0;
	for(i = 0; i < NUM_CTRLS; ++i )
	{
		ctrlVals[i] = 0xff;
	}
}

void
startPgmChange()
{
	do
	{
		destPgm = pgms[random(0,sizeof(pgms))];
		change = abs(destPgm - curPgm);
	} while ( !change );
	timeVal = 2000 / abs(destPgm - curPgm); 
	setTimer(PGM_TIMER,timeVal);
	setTimer(DEBOUNCE_TIMER,DEBOUNCE_TIME);
	pgmChangeOk = 0;
}

void
checkCtrl()
{
	RB0 = pgmChangeOk;
	for ( i = 0; i < NUM_CTRLS; ++i )
	{
		newVal = getA2D(i);	
		if ( newVal != ctrlVals[i] )
		{
			RC5 = 1;
			ctrlVals[i] = newVal;
#if 0
			putCtrl(i+CTRL_BASE,newVal,0);
#endif
		}
#if 0
		if ( (ctrlVals[LIGHT] > LIGHT_MIN)
			|| (ctrlVals[FLEX] > FLEX_MIN ) )
#else
		if( ctrlVals[FLEX] > ctrlVals[FLEX_MIN_VAL] ) 
#endif
		{
			if ( zero )
			{
				zero = 0;
				if ( pgmChangeOk )
				{
					startPgmChange();
				}
			}
		}
		else
		{
			zero = 1;
		}
	}
}

void
checkPgm()
{
	if ( !pgmChangeOk )
	{
		if ( timerReady(PGM_TIMER) )
		{
			RC5 = 1;
			if ( curPgm == destPgm )
			{
				pgmChangeOk = 1;
			}
			else if ( curPgm < destPgm )
			{
				++curPgm;
			}
			else
			{
				--curPgm;
			}
			setTimer(PGM_TIMER,timeVal);
			putPgm(curPgm,0);
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
	initA2D();
	initCtrl();
	ei();	// enable interrupts
	setTimer(DEBOUNCE_TIMER,DEBOUNCE_TIME);
	while ( 1 )
	{
		CLRWDT();
		RC4 = 0;
		RC5 = 0;
		checkA2D();
		if ( timerReady(DEBOUNCE_TIMER) )
			checkCtrl();
		if ( oneKhz )
		{
			oneKhz = 0;
			RC4 = 1;
			checkTimers();
			checkPgm();
		}
	}
}
