#include <pic.h>
// #include <sys.h>
#include "types.h"

#include "rtclock.h"
#include "nixie74141.h"


const uint8_t millitCount = 104;
const uint8_t milliCount = 30;
#define BLANK_COUNT 80
#define BLANK_COUNT_LOW	40
#define REFRESH_COUNT	40
uint8_t	milli = 1;
uint8_t oneKhz;
uint8_t blank;
uint8_t refresh;
uint8_t inBlank;
uint16_t nixieCount;
uint16_t duty1;
uint16_t dutyTimer1;
uint16_t periodTimer;
uint8_t	secFlag;
uint16_t	secCount;
uint16_t errorCount;

void interrupt
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = millitCount;
#if 1
		if ( !(--periodTimer) )
		{
			periodTimer = PERIOD;
			if ( !duty1 ) {
				RC7 = 0;
			} else {
				RC7 = 1;
				dutyTimer1 = duty1;
			}
		}
		else 
		{
			if ( RC7 && duty1 < PERIOD ) {
				if ( !--dutyTimer1 )
					RC7 = 0;
			}
		}
#endif
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
	if ( INTF ) {
		INTF = 0;
		oneKhz = 1;
		RA0 = RA0 ? 0 : 1;
		if ( ++secCount == 1024 ) {
			secFlag = 1;
			secCount = 0;
		}
	}
}

void
initInterrupt()
{
	milli = milliCount;
	T0IE = 1;	// enable timer 0 interrupt
	T0IF = 0;
	INTE = 1;
}

void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x01;
	TRISA = 0x00;
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
	oneKhz = 0;
	secCount = 0;
	secFlag = 0;
}

// Begin nixie code

#define NUM_NIXIES 4
uint8_t nixieRefreshCount = 0;
uint8_t nixieVals[4];

void
nixieClearSelect() {
	RB4 = 0;
	RB5 = 0;
	RC2 = 0;
	RC0 = 0;
}

void
nixieSet(uint8_t num,uint8_t val) {
	PORTB &= 0xF1;
	RA1 = 0;
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
			RC0 = 1;
			break;
		default:
			break;
	}	
	PORTB |= val & 0xE ;
	RA1 = val & 1;
}


void
nixieRefresh() {
	nixieSet(nixieRefreshCount,nixieVals[nixieRefreshCount]);
	if ( ++nixieRefreshCount == NUM_NIXIES ) {
		nixieRefreshCount = 0;
	}
}	


uint16_t	ft;
uint16_t 	ct;


void
main()
{
	uint8_t check = 0;
	init();
	initInterrupt();
	versionInit();
	initButtons();
	rtcInit();
	ei();	// enable interrupts
	
	while ( 1 )
	{
		CLRWDT();
		if ( blank ) {
			blank = 0;
			nixieClearSelect();
		}
		if ( refresh ) {
			refresh = 0;
			nixieRefresh();
		}
		if ( rtcCheck() ) {
			check = 1;
			setHours();
			setSeconds();
	 		setMinutes();
		}
		if ( oneKhz )
		{
			oneKhz = 0;
			checkButtons();
			
			if (secFlag) {
				secFlag = 0;
#if 0
				if ( !check ) {
					++errorCount;
					if ( ++seconds == 60 ) {
						seconds = 0;
						if ( ++minutes == 60 ) {
							minutes = 0;
							if ( ++hours == 13 ) {
								hours = 1;
							}
						}
					}
				}
#endif
				check = 0;
				displayHours();
				displayMinutes();
				displaySeconds();
				rtcStart();
			}
		}
	}
}

