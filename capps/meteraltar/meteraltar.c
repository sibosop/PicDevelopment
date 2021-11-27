#include <pic.h>
#include <sys.h>
#include "types.h"

#include "rtclock.h"
#include "serial.h"
#include "display.h"
#include "leds.h"
#include "pseudorand.h"
#include "a2d.h"
#include "rtcbutton.h"
#include "meteraltar.h"

DECLARE_A2D_VALS(5);
UInt8 oneKhz;
UInt8 secFlag;
UInt16 secCount;
UInt8  hour;
UInt8  minute;
UInt8  second;

#define NUM_LEDS	60
Rgb zero = { 0, 0, 0};
Rgb hourColor =   { 0xff,0,0 };
Rgb minuteColor = { 0, 0xff, 0 };
Rgb secondColor = { 0, 0, 0xff };
Rgb *currentColor = 0;


UInt8 hourPos;
UInt8 minutePos;
UInt8 secondPos;
Boolean showMinute;

void setHourColor() {
	currentColor = &hourColor;
}
void setMinuteColor() {
	currentColor = &minuteColor;
}
void setSecondColor() {
	currentColor = &secondColor;
}

void setCurrentColor() {
	UInt8 pos;
	if ( currentColor == &hourColor ) {
		pos = hourPos;
	} else if ( currentColor == &minuteColor ) {
		pos = minutePos;
	} else {
		pos = secondPos;
	}
	ledSet(pos,currentColor);
}
		
void setNoColor() {
	currentColor = 0;
}




void interrupt
interruptHandler()
{
	if ( INTF ) {
		INTF = 0;
		oneKhz = 1;
		RA0 = RA0 ? 0 : 1;
		if ( ++secCount == 1024 ) {
			secFlag = 1;
			secCount = 0;
		}
	}
	checkTransmit();
}

void
initInterrupt()
{
	T0IE = 0;	// enable timer 0 interrupt
	T0IF = 0;
	INTE = 1;
}


void
init()
{
	PORTC = 0;
	TRISC = 0x00;	// make C outputs
	TRISB = 0x01;
	TRISA = 0xFF;	// make A outputs
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	oneKhz = 0;
	secCount = 0;
	secFlag = 0;
}

void
setHours() {
	hour = rtcGetHour();
}

void
setMinutes() {
	minute = rtcGetMin();
}

void
setSeconds() {
	second = rtcGetSecond();
}

void
displayTime() {
	displayHours();
	displayMinutes();
	displaySeconds();	
}



void
displayHours() {
	UInt8 tmp;
	tmp = (bcd2Bin(hour) % 12) * 5;
	if ( tmp != hourPos ) {
		ledSet(hourPos,&zero);
		hourPos = tmp;
	}
}
void
displayMinutes() {
	UInt8	tmp;
	tmp = bcd2Bin(minute);
	if ( tmp != minutePos ) {
		ledSet(minutePos,&zero);
		minutePos = tmp;
	}
}

void
dspTime() {
	dspSetCursor(0,1);
	dspByte(hour);
	dspChar(':');
	dspByte(minute);
	dspChar(':');
	dspByte(second);
}


void
displaySeconds() {
	UInt8 tmp;
	dspTime();
	tmp = bcd2Bin(second);
	if ( tmp != secondPos ) {
/*
		if ( secondPos != hourPos && secondPos != minutePos)
			ledSet(secondPos,&zero);
		if ( tmp != hourPos && tmp != minutePos )
			ledSet(tmp,&secondColor);
*/
		ledSet(secondPos,&zero);
		secondPos = tmp;
	}
	if ( minutePos == hourPos ) {
		if ( showMinute ) {
			ledSet(minutePos,&minuteColor);
			showMinute = 0;
		} else {
			showMinute = 1;
			ledSet(minutePos,&hourColor);
		}
	} else {
		ledSet(hourPos,&hourColor);
		ledSet(minutePos,&minuteColor);
	}
	ledSet(secondPos,&secondColor);
}

#define SPEED 100
void
main()
{
	UInt8 check = 0;
	UInt16	count = SPEED;
	init();
	initButtons();
	initInterrupt();
	initSerial(0,1);
	rtcInit();
	initDisplay();
	initA2D();	
	
	ledInit(NUM_LEDS);
	ei();	// enable interrupts


	while ( 1 )
	{
		CLRWDT();
		ledCheck();
		checkA2D();

		if ( rtcCheck() ) {
			check = 1;
			setHours();
			setSeconds();
	 		setMinutes();
			displayTime();
		}
		if ( oneKhz )
		{
			checkButtons();
			oneKhz = 0;
			if ( --count == 0 ) {
				count = SPEED;
				if ( currentColor ) {
					currentColor->r = getA2D(1);
					currentColor->g = getA2D(2);
					currentColor->b = getA2D(3);
				}
				dspCursorHome();
				if ( currentColor ) {
					dspByte(currentColor->r);
					dspChar(' ');
					dspByte(currentColor->g);
					dspChar(' ');
					dspByte(currentColor->b);
					setCurrentColor();
				}
			}				
			if (secFlag) {
				secFlag = 0;
				check = 0;
				rtcStart();
			}
		}
	}
}

