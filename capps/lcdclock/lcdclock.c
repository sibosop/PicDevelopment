#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
#include "pseudorand.h"
#include "lcd.h"
#include "lcdclock.h"


DECLARE_INT_FLAGS;
DECLARE_TIMERS(4);



bank2 UInt8	numWords;
#ifdef KLUDGE
UInt8	kludge;
#endif

void
init()
{
	PORTC = 0;
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, 
	PORTB = 0;
	TRISB = 0x00;	// B is all outputs
	PORTA = 0;
	TRISA = 0xb;	// 0,1,3 a-d , 2,4,5 outputs 
	
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
	ADCON0 = 0;
	ADCON1 = 6;		// set all a/d ports to digital


	for ( numWords=0; words[numWords] != NULL ; ++numWords)
			;
	numWords >>= 1;
#ifdef KLUDGE
	kludge = 0;
#endif
}

const char *cp;
#if 1
UInt8
strLen(const char *cp)
{
	UInt8 rval = 0;
	for (;*cp;++cp,++rval);
	return rval;
}
#endif

UInt8 w = 0;
UInt8 rep = 1;
void
putWords()
{
	//UInt8	w = 0;
	UInt8	ts;
	char	pos;
	UInt8	ws;
	const char	*cp;

	if ( !--rep ) { 
		w = pseudoRand(0,numWords);	
		rep = pseudoRand(1,10);
	}
	ts = pseudoRand(0,4);
	cp = words[w];
	pos = *cp++;
	ws = pseudoRand(0,8-strLen(cp));
	lcdSetCursor(pos == '+' ? ts : ws,0);
	lcdSendMessage(pos == '+' ? "time" : cp);
	lcdSetCursor((pos == '-' ? ts : ws)+8,0);
	lcdSendMessage(pos == '-' ? "time" : cp);
}

char clock[9];
UInt8 seconds;
UInt8 minutes;
UInt8 hours;

void 
initClock() {
	seconds = 0;
	minutes = 0;
	hours = 1;
	clock[2] = ':';
	clock[5] = ':';
	clock[8] = 0;
}

void
put2Dec(char *cp,UInt8 v) {
	cp[0] = '0' + (v/10);
	cp[1] = '0' + (v%10);
}

void
printClock() {
	put2Dec(clock,hours);
	put2Dec(&clock[3],minutes);
	put2Dec(&clock[6],seconds);
	lcdSetCursor(4,1);
	lcdSendMessage(clock);
}

void
updateClock() {
	if ( ++seconds == 60 ) {
		seconds = 0;
		if ( ++minutes == 60) {
			minutes = 0;
#ifdef KLUDGE
			if ( ++kludge == 2 ) {
				++seconds;
				kludge = 0;
			}
#endif
			if ( ++hours == 13 ) {
				hours = 1;
			}
		}
	}
} 

UInt8 minCount;
void
checkButtons() {
	if ( timerReady(BUTT_TIMER1) ) {
		if ( !RC2 ) {
			seconds = 0;
			if ( ++minutes == 60 ) 
				minutes = 0;
			printClock();
			if ( minCount < 5 ) {
				++minCount;
				setTimer(BUTT_TIMER1,500);
			} else {
				setTimer(BUTT_TIMER1,100);
			}
		} else {
			minCount = 0;
		} 
	} 
	if ( timerReady(BUTT_TIMER2) ) {
		if ( !RC3 ) {
			if ( ++hours == 13 ) 
				hours = 1;
			printClock();
			setTimer(BUTT_TIMER2,500);
		}
	}
}

bank2 UInt8 dispCount;
bank2 UInt8	currentPhrase;
bank2 UInt8 numPhrases;

void
initPhrase() {
	dispCount = 0;
}

void
checkDisp() {

	UInt8 i;
	const char *cp;

	for( i = 0; phrases[i].hour ; ++i ) {
		if ( phrases[i].hour == hours 
				&& phrases[i].minute == minutes
				&& phrases[i].second == seconds
			) {
			dispCount = 10;
			lcdSetCursor(0,0);
			for ( cp = phrases[i].phrase; *cp != '*'; ++cp ) {
				lcdSendData(*cp);
			}
			++cp;
			lcdSetCursor(0,1);
			lcdSendMessage(cp);
		}
	}
}

void 
doSecond() {
	RC6 = RC6 ? 0 : 1;
	updateClock();
	if ( !dispCount ) {
		lcdClearDisplay();		
		putWords();
		printClock();
		checkDisp();
	} else {
		--dispCount;
	}
}

void
main()
{

	UInt8 	state = 0;
	
	init();
	initInterrupt(1,1);
	initTimers();
	pseudoSeed(66);
	initClock();
	//initPhrase();
	ei();	// enable interrupts
	while ( state < 5 )
	{
		CLRWDT();
		// lcd initialization sequence
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
			if ( timerReady(INIT_TIMER) )
			{
				switch ( state )
				{
					case 0:
						setTimer(INIT_TIMER,30);		// wait 30 milliseconds before initializing lcd
						break;
					case 1:
					case 2:
					case 3:
						lcdInitDisplay1();
						setTimer(INIT_TIMER,30);
						break;
					case 4:
						lcdInitDisplay2();
						break;
				}
				++state;
			}
		}
	}
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
			checkButtons();
		}
		if ( pinInt ) 
		{	
			--pinInt;
			doSecond();	
		}
	}
}
