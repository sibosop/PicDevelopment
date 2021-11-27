#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "midi.h"
#include "timers.h"


DECLARE_TIMERS(4);
DECLARE_INT_FLAGS;


#if defined(_16F876)
#include "serial.h"
#endif

extern void zeroCross();
extern void checkDimCounter();

DECLARE_QUEUES(16,16);

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);



Boolean clockFlag;

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x01;	// B0 is 60hz interrupt, other pins are dimmer out
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x88;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;

}

#define NUM_DIMMERS	4

bank2   UInt8	dimCnts[NUM_DIMMERS];
bank2   UInt8	dimCounter;
bank2   UInt8 	kcount;
bank2   UInt8 	i;
bank1	curI;

void 
initDimmers()
{
	dimCounter = 0;
	for ( i = 0; i < NUM_DIMMERS; ++i )
	{
		dimCnts[i] = 0x80;
	}
}

#if 0
void
handleChannelPressure(void)
{
	dimCnts[curI] = 0x80 - mbyte1;
}

void
handleNoteOn(void)
{
	if ( mbyte2 )
	{
		curI = mbyte1 % NUM_DIMMERS;
		dimCnts[curI] = 0x80 - mbyte2;
	}
	else
	{
		dimCnts[mbyte1 % NUM_DIMMERS] = 0x80;
	}
}
#endif


#define MAX_VAL 110
void 
handleControlChange (void)
{
	curI = mbyte1 % NUM_DIMMERS;
	mbyte2 = mbyte2 > MAX_VAL ? 0 : MAX_VAL - mbyte2;
	dimCnts[curI] = 0x80 - mbyte2;
}



void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = 5;
		RC4 = RC4 ? 0 : 1;
		if ( dimCounter != 255 )
			++dimCounter;


		for ( i = 0; i < NUM_DIMMERS; ++i )
		{
			if ( dimCnts[i] < dimCounter )
			{
				if ( i > 1 )
					PORTB |= 1 << (i+2);
				else
					PORTB |= 1 << (i+1);
			}
		}
		if ( ++kcount == 18 )
		{
			oneKhz = 1;
			kcount = 0;
		}
	}
	if ( INTF )
	{
		INTF = 0;
		pinInt = 1;
		dimCounter = 0;
		for ( i = 0; i < NUM_DIMMERS; ++i )
		{
			if ( dimCnts[i] )
			{
				if ( i > 1 )
					PORTB &= ~(1 << (i+2));
				else
					PORTB &= ~(1 << (i+1));
			}
		}
	}
	checkTransmit();
	checkReceive();
}

void
initInterrupt(UInt8 rb0Flag,UInt8 timerFlag)
{
	kcount = 0;
	if ( timerFlag )
	{
		T0IE = 1;	// enable timer 0 interrupt
	}
	if ( rb0Flag ) INTE = 1;
	
	
}

UInt8	times[4];

void
main()
{
	curI = 0;

	init();
	initInterrupt(1,1);
	initSerial(1,1);
	initmparse();
	initDimmers();
	initTimers();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( pinInt )
		{
			pinInt = 0;
		}


		if ( oneKhz )
		{
			checkTimers();
			oneKhz = 0;
			RC5 = RC5 ? 0 : 1;
		}

		if ( CharReady )
		{
			CharReady = 0;
			checkmparse();
		}
	}
}
