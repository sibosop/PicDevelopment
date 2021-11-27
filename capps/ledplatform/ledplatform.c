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
#include "ledplatform.h"
#include "eeprom.h"

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

#define REVERSE

#ifdef REVERSE
#define LED_SET(p,c) ledSet((60-p)%60,c)
#else
#define LED_SET(p,c) ledSet(p,c)
#endif

void writeColors() {
	UInt8 addr = 0;
	EEPROM_WRITE(addr++,0xDD);
	EEPROM_WRITE(addr++,hourColor.r);
	EEPROM_WRITE(addr++,hourColor.g);
	EEPROM_WRITE(addr++,hourColor.b);
	EEPROM_WRITE(addr++,minuteColor.r);
	EEPROM_WRITE(addr++,minuteColor.g);
	EEPROM_WRITE(addr++,minuteColor.b);
	EEPROM_WRITE(addr++,secondColor.r);
	EEPROM_WRITE(addr++,secondColor.g);
	EEPROM_WRITE(addr++,secondColor.b);
	EEPROM_WRITE(addr++,zero.r);
	EEPROM_WRITE(addr++,zero.g);
	EEPROM_WRITE(addr++,zero.b);
	
}

void readColors() {
	UInt8 addr = 0;
	if ( EEPROM_READ(addr++) != 0xDD )
		return;
	
	hourColor.r = EEPROM_READ(addr++);
	hourColor.g = EEPROM_READ(addr++);
	hourColor.b = EEPROM_READ(addr++);
	minuteColor.r = EEPROM_READ(addr++);
	minuteColor.g = EEPROM_READ(addr++);
	minuteColor.b = EEPROM_READ(addr++);
	secondColor.r = EEPROM_READ(addr++);
	secondColor.g = EEPROM_READ(addr++);
	secondColor.b = EEPROM_READ(addr++);
	zero.r = EEPROM_READ(addr++);
	zero.g = EEPROM_READ(addr++);
	zero.b = EEPROM_READ(addr++);
}


void setHourColor() {
	currentColor = &hourColor;
}
void setMinuteColor() {
	currentColor = &minuteColor;
}
void setSecondColor() {
	currentColor = &secondColor;
}

void setZeroColor() {
	currentColor = &zero;
}

void setZero() {
	UInt8 pos;
	for ( pos = 0; pos < NUM_LEDS; pos++ ) {
			LED_SET(pos,&zero);
	}
}

void setCurrentColor() {
	UInt8 pos;
	if ( currentColor == &hourColor ) {
		pos = hourPos;
	} else if ( currentColor == &minuteColor ) {
		pos = minutePos;
	} else if ( currentColor == &secondColor ) {
		pos = secondPos;
	} else {
		setZero();
		return;
	}
	LED_SET(pos,currentColor);
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
	readColors();
	setZero();
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
		LED_SET(hourPos,&zero);
		hourPos = tmp;
	}
}
void
displayMinutes() {
	UInt8	tmp;
	tmp = bcd2Bin(minute);
	if ( tmp != minutePos ) {
		LED_SET(minutePos,&zero);
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
		LED_SET(secondPos,&zero);
		secondPos = tmp;
	}
	if ( minutePos == hourPos ) {
		if ( showMinute ) {
			LED_SET(minutePos,&minuteColor);
			showMinute = 0;
		} else {
			showMinute = 1;
			LED_SET(minutePos,&hourColor);
		}
	} else {
		LED_SET(hourPos,&hourColor);
		LED_SET(minutePos,&minuteColor);
	}
	LED_SET(secondPos,&secondColor);
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

