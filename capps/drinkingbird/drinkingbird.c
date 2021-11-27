#include <pic.h>
#include <sys.h>


#include "midi.h"
#include "a2d.h"
#include "serial.h"
//#define USE_SERIAL

DECLARE_A2D_REF;





__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);


#define MILLIT	104


#define LOW_SET 	8
#define HIGH_SET 	38
#define APP_TIMER	1
#define NUM_SERVOS	4
#define PERIOD_TIMER	570
#define MIN_VAL 	100
#define MAX_VAL	    180
#define NUM_CTRLS	NUM_SERVOS
#define CTRL_BASE	4
#define CTRL_DEBOUNCE_TIME	100
#define JITTER_RANGE	1

UInt8	ctrlVals[NUM_CTRLS];
UInt8	ctrlTimer;
UInt8	duties[NUM_SERVOS];
UInt8	dutyTimers[NUM_SERVOS];
UInt8 	oneKhz;
UInt16 	periodTimer;
UInt8	servoCnt;
UInt8	downCount;

void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = MILLIT;
		oneKhz = 1;
		if ( !(--periodTimer)  )
		{
			downCount = 0;
			periodTimer = PERIOD_TIMER;
			PORTB |= (1 << NUM_SERVOS) - 1;	
			for ( servoCnt = 0; servoCnt < NUM_SERVOS; ++servoCnt )	
				dutyTimers[servoCnt] = duties[servoCnt];
		}
		else
		{
			if ( downCount != NUM_SERVOS )
			{
				for ( servoCnt = 0; servoCnt < NUM_SERVOS; ++servoCnt )
				{

					if ( (PORTB & (1<<servoCnt)) )
					{
						if (!(--dutyTimers[servoCnt]))
						{
							++downCount;
							PORTB &= ~(1 << servoCnt);
						}
					}
				}
			}
		}
	}

#ifdef USE_SERIAL
	checkTransmit();
	checkReceive();
#endif
}

void
initInterrupt()
{
	TMR0 = MILLIT;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
}




void 
initCtrl()
{
	UInt8 i;
	for(i = 0; i < NUM_CTRLS; ++i )
	{
		ctrlVals[i] = 0xff;
	}
	ctrlTimer = CTRL_DEBOUNCE_TIME;
}

UInt8
rangeCheck(UInt8 v, UInt8 cv)
{
#if 1
	if ( v > (cv+JITTER_RANGE) )
		return v;

	if ( v < (cv-JITTER_RANGE) )
		return v;
	return cv;
#else
	return v;
#endif
}

void
checkCtrl()
{
	if ( !--ctrlTimer )
	{
		UInt8 i;
		UInt8 newVal;
		UInt8 cv;
		for ( i = 0; i < NUM_CTRLS; ++i )
		{
			newVal = getA2D(i>=3 ? i+1 : i);
			newVal = newVal > MIN_VAL ? newVal-MIN_VAL : 0 ;	
			//newVal >>= 1;
			cv = rangeCheck(newVal,ctrlVals[i]);
			if ( cv != ctrlVals[i] )
			{
				RC5 = 1;
				
				ctrlVals[i] = cv;
				putCtrl(i+CTRL_BASE,cv,0);
			}
		}
		ctrlTimer = CTRL_DEBOUNCE_TIME;
	}
}

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	TRISA = 0xFF;
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
}



void
initServos()
{
	UInt8 servo;
	for (servo = 0; servo < NUM_SERVOS; ++servo )
		duties[servo] = LOW_SET;
}

void
checkServos()
{
	UInt8 i;
	UInt8 newVal;
	UInt8 dv;
	for ( i = 0; i < NUM_SERVOS; ++i )
	{
		newVal = getA2D(i>=3 ? i+1 : i);
		newVal -= MIN_VAL;
		newVal /= 2;
		newVal += LOW_SET;
		dv = rangeCheck(newVal,duties[i]);
		if (dv != duties[i] )
		{		
			duties[i] = dv;
		}
	}
}



void
main()
{
	UInt16 appTimer;
	
	
	init();
	initInterrupt(0,1);
	initA2D();
	initSerial(0,1);
	//initmparse();
	//initTimers();
	//initRandom();
	initServos();
	initCtrl();
	

	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		checkA2D();
		
		if ( oneKhz )
		{
			if ( appTimer < 20000 )
			{
				++appTimer;
			}
			else
			{
				oneKhz = 0;
				checkServos();
				checkCtrl();
			}
		}
	}
}
