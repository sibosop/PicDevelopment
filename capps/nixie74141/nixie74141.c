#include <pic.h>
#include <sys.h>
#include "types.h"
#include "a2d.h"

//#define BONG_HOURS
#include "nixie74141.h"


const UInt8 millitCount = 104;
const UInt8 milliCount = 30;
#define BLANK_COUNT 80
#define BLANK_COUNT_LOW	40
#define REFRESH_COUNT	40
UInt8	milli = 1;
Boolean	pinInt;
Boolean oneKhz;
Boolean blank;
Boolean refresh;
Boolean inBlank;
UInt16 nixieCount;
UInt16 duty0;
UInt16 duty1;
UInt16 dutyTimer0;
UInt16 dutyTimer1;
UInt16 periodTimer;
UInt8  meterCnt;

UInt16 mramp;
UInt16 hramp;
UInt8  rflag;
UInt8  sflag;
UInt16 sramp;


UInt8	meterCnt;

int
getSecondPad()
{
	return 0;
}




void interrupt
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
	//	ei();
		TMR0 = millitCount;
#if 1
		if ( !(--periodTimer) )
		{
			periodTimer = PERIOD;
		
#if 0
			if ( !duty0 ) {
				RC6 = 0;
			} else {
				RC6 = 1;
				dutyTimer0 = duty0;
			}
#endif
			if ( !duty1 ) {
				RC7 = 0;
			} else {
				RC7 = 1;
				dutyTimer1 = duty1;
			}

		}
		else 
		{
#if 0
			if ( RC6 && duty0 < PERIOD ) {
				if ( !--dutyTimer0 )
					RC6 = 0;
			}
#endif
			if ( RC7 && duty1 < PERIOD ) {
				if ( !--dutyTimer1 )
					RC7 = 0;
			}
		}
#endif
	
		if ( !(--milli) )
		{
			milli = milliCount;
			oneKhz = 1;
			
		}
#if 1
		if ( !--nixieCount ) {
			if ( inBlank ) {
				blank = 1;
				inBlank = 0;
				nixieCount = REFRESH_COUNT;
			} else {
				refresh = 1;
				inBlank = 1;
				nixieCount = BLANK_COUNT;
			}
		}
#endif
	}

	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1L = 0;
		TMR1IF = 0;
		pinInt = 1;
	}
}

void
initInterrupt()
{
	milli = milliCount;
	T0IE = 1;	// enable timer 0 interrupt
	T0IF = 0;
	// one second timer
	pinInt = 0;
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;
	
}

#ifdef BONG_HOURS
UInt8	solCount;
UInt16 	solWait;
UInt8	bongCount;
#define SOL_ON_TIME	200
#define SOLENOID	RC5
#define SOL_INTERVAL	3
#endif
void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	TRISA = 0x01;  // a2d needs port to be input
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	nixieCount = BLANK_COUNT;
	inBlank = 1;
	blank = 0;
	refresh = 0;
	pinInt = 0;
	oneKhz = 0;
#ifdef BONG_HOURS
	SOLENOID = 0;
	solCount = 3;
	solWait = 0;
#endif
//	hourFlag = 0;
}

// Begin nixie code

#define NUM_NIXIES 4
UInt8 nixieRefreshCount = 0;
UInt8 nixieVals[4];

void
nixieClearSelect() {
	RB4 = 0;
	RB5 = 0;
	RC2 = 0;
	RC3 = 0;
}

void
nixieSet(UInt8 num,UInt8 val) {
	PORTB &= 0xF0;
	switch ( num ) {
		case 0:
			RB4 = 1;
			break;
		case 1:
			RB5 = 1;
			break;
		case 2:
			RC2 = 1;
			break;
		case 3:
			RC3 = 1;
			break;
		default:
			break;
	}	
	PORTB |= val ;
}


void
nixieRefresh() {
	nixieSet(nixieRefreshCount,nixieVals[nixieRefreshCount]);
	if ( ++nixieRefreshCount == NUM_NIXIES ) {
		nixieRefreshCount = 0;
	}
}	


UInt16	ft;
UInt16 	ct;

DECLARE_A2D_VALS(1);

void
main()
{
	
	init();
	initInterrupt();
	versionInit();
	initButtons();
	initA2D();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		checkA2D();
		if ( blank ) {
			blank = 0;
			nixieClearSelect();
		}
		if ( refresh ) {
			refresh = 0;
			nixieRefresh();
		}
		if ( oneKhz )
		{
			oneKhz = 0;
			//doRamp();
			checkButtons();	
#ifdef BONG_HOURS
			if ( solWait && !--solWait ) {
				SOLENOID = 0;
				--bongCount;
			}	
#endif
		}
		if (pinInt) {
			pinInt = 0;
			doClock();

#ifdef BONG_HOURS			
			if ( hourFlag ) {
				hourFlag = 0;
				bongCount = hours;
				solCount = 1;
			}

			if ( bongCount ) {
				if ( solCount && !--solCount ) {
					solWait = SOL_ON_TIME;
					solCount = SOL_INTERVAL;
					SOLENOID = 1;
				}
			}
#endif
		}		
	}
}

