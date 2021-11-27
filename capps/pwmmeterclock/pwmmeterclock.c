
#include "types.h"
#include "pic.h"
#include "timers.h"
#include "mclock.h"


UInt16	duties[NUM_METERS];
UInt16	dutyTimers[NUM_METERS];
UInt8 	oneKhz;
UInt16 	periodTimer;
UInt8	servoCnt;
UInt8 pinInt = 0;

UInt16	mramp;
UInt16	hramp;
UInt8	rflag;
UInt8	sflag;
UInt16	sramp;

Int16	destVal;
Int16 	destInt;
UInt8	secFlag = 0;
DECLARE_TIMERS(4);

void interrupt 
interruptHandler()
{
	UInt8 mask;
	// one second timer interrupt
	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1IF = 0;
		pinInt = 1;
	}
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = MILLIT;
		oneKhz = 1;
		if ( !(--periodTimer)  )
		{
			periodTimer = PERIOD;
			PORTB &= ~((1 << NUM_METERS) - 1);
			for ( mask = 1, servoCnt = 0; servoCnt < NUM_METERS; ++servoCnt, mask <<= 1 ) {	
				dutyTimers[servoCnt] = duties[servoCnt];
				if ( duties[servoCnt] ) 
					PORTB |= mask;
			}
		}
		else
		{
 			for ( mask = 1, servoCnt = 0; servoCnt < NUM_METERS; ++servoCnt, mask <<= 1 )
			{
				if ( PORTB & mask )
				{
					if ( dutyTimers[servoCnt] >= PERIOD )
						continue;

					if (!(--dutyTimers[servoCnt]))
						PORTB &= ~mask;
				}
			}
		}
	}	
}

void
initInterrupt()
{
	TMR0 = MILLIT;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
	pinInt = 0;
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;
}

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
}


main()
{ 
	init();
	versionInit();
	initInterrupt();
	initButtons();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			checkTimers();
			oneKhz = 0;	
			checkButtons();
			if ( timerReady(RAMP_TIMER) ) {
				doRamp();
			}
		}
		if ( pinInt )
		{
			pinInt = 0;
			doClock();
		}
	}
}
