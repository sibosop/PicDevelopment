#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "a2d.h"
#include "lcd.h"
#include "lcdtext.h"
#include "linefunc.h"


DECLARE_INT_FLAGS;
DECLARE_TIMERS(4);



bank2 UInt8	numWords;
UInt8 scrollDone;
UInt8 doneCount;


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
	scrollDone = 1;
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

void
initBounce()
{	
	scrollDone = 1;
	lcdDisableShift();	
	lcdSetCursor(0,0);
}

void
bounceLoop()
{
#if 1
	UInt8	w;
	UInt8	l;
	UInt8	s;
	UInt8	l2;
	UInt8	s2;
	


	if ( timerReady(WORD1_TIMER) )
	{
		setTimer(WORD1_TIMER,random(100,2000));
		lcdClearDisplay();
		w = random(0,numWords);
		w <<= 1;
		l = strLen(words[w]);
		s = random(0,16-l);
		l2 = strLen(words[w+1]);
		s2 = random(0,16-l2);
		lcdSetCursor(s,0);
		lcdSendMessage(words[w]);
		lcdSetCursor(s2,1);
		lcdSendMessage(words[w+1]);
	}
#endif
}


bank3 UInt8 numQuotes;
bank3 UInt8 curQuote;
bank3 UInt16 qIndex;
bank3 UInt8 curCount;
bank3 UInt8 spaceMode;


void
initScroll()
{
	UInt8 i;
	scrollDone = 0;
	lcdClearDisplay();
	lcdSetCursor(0,0);
	lcdEnableShift(1);
	for ( numQuotes = 0; quotes[numQuotes]; ++numQuotes )
		;

	qIndex = 0;
	curQuote = random(0,numQuotes);
	curCount = 0;
	spaceMode = 0;

	lcdSetCursor(0,1);
	for ( i = 0; i < 40; i += 8 )
	{
		lcdSendData('*');
		lcdSendData('_');
		lcdSendData('_');
		lcdSendData('_');
		lcdSendData('_');
		lcdSendData('_');
		lcdSendData('_');
		lcdSendData('_');		
		//lcdSendData('_');
		//lcdSendData('_');
		
	}
	lcdSetCursor(0,0);

}

void
scrollLoop()
{
	const char *cp;
	if ( timerReady(SCROLL_TIMER) )
	{
		
		if ( spaceMode )
		{
		
			if ( --qIndex )
			{
				if ( doneCount >= 40 )
					scrollDone = 1;
				else
					++doneCount;

				lcdSendData(' ');
				++curCount;
				setTimer(SCROLL_TIMER,150);
			}
			else
			{
				spaceMode = 0;
				qIndex = 0;
				scrollDone = 0;
			}
		}
		else
		{
			cp = &quotes[curQuote][qIndex];
			if (!*cp)
			{
				curQuote = random(0,numQuotes);
				qIndex = random(40,100);
				doneCount = 0;
				spaceMode = 1;
			}
			else
			{
				scrollDone = 0;
				lcdSendData(*cp);
				++qIndex;
				++curCount;
				setTimer(SCROLL_TIMER,150);
			}
		}

		if ( curCount == 40 )
		{
			curCount = 0;
			lcdSetCursor(0,0);
		}
	}
}

void
main()
{

	UInt8 	state = 0;
	
	init();
	initInterrupt(0,1);
	//initSerial(0,1);
	initTimers();
	initRandom();
	//initA2D();
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
	//initScroll();
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
			if ( timerReady(APP_TIMER ))
			{
				if ( scrollDone )
				{
					setTimer(APP_TIMER,random(10000,20000));
					state = random(0,2);
					switch(state)
					{
						case 0:
							initScroll();
							break;
						case 1:
						default:
							initBounce();
							break;
					}
				}
			}
			
			switch(state)
			{
				case 0:
					scrollLoop();
					break;
				default:
				case 1:
					bounceLoop();
					break;
			}
		}
	}
}
