#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "serial.h"
#include "midi.h"
#include "timers.h"
#include "random.h"
#include "a2d.h"
#include "lcd.h"
#include "dimctrl.h"
#include "linefunc.h"


DECLARE_QUEUES(1,16);
DECLARE_A2D_VALS(3);
DECLARE_INT_FLAGS;
DECLARE_TIMERS(3);

#define NOTE_TIMER	2


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
}

extern void checkMenus();

void
main()
{
	//UInt16 bitCount = 0;
	UInt8 	state = 0;
	//UInt8	col = 0;
	UInt8 	displayFlag = 0;
	//UInt8 	lastNote = 0;
	//UInt8	lastVel = 0;

	init();
	initInterrupt(0,1);
	initSerial(0,1);
	initTimers();
	initRandom();
	initA2D();
	initLines();
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
		checkA2D();
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
#if 1
			if ( timerReady(INPUT_TIMER) )
				checkMenus();
#endif
			if ( newNote )
			{
				newNote = 0;
				putNote(currentNote, currentVel, 0);
			}
			if ( !displayFlag )
			{
				displayFlag = 1;
				lcdSetCursor(0,0);
				lcdSendMessage("Sibosop");
			}

			if ( timerReady(DIMMER_TIMER) ) 
			{
				setTimer(DIMMER_TIMER,100);
				checkLines();
			}
		}
	}
}
