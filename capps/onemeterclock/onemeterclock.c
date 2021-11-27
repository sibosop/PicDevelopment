

#include <pic.h>
#include <sys.h>

#include "types.h"
#include "serial.h"
#include "interrupt.h"
#include "timers.h"
#include "onemeterclock.h"
#include "button_spec.h"

#define RMS_CLOCK


#define LED_RAMP_DELAY 2
#define MILLI	1
#define MILLIT	00

TWEEK_MILLI(MILLI,MILLIT);


#ifdef RMS_CLOCK
#define LED_MIN	100
#define LED_MAX 255
#endif

#ifdef SETTINGS_CLOCK
const UInt8 const hourLookup[] = {
	00,		// 0 place holder
	20,		// 1
	40, 	// 2
	60, 	// 3
	80,		// 4
	103,	// 5
	125,	// 6
	145,	// 7
	167,	// 8
	190,	// 9
	209,	// 10
	230,	// 11
	248		// 12
};

const int const minuteLookup[] = {
// 0 - 9
  0,  4, 8, 12, 16, 20, 24, 28, 32, 36,
// 10 - 19
 40, 44, 48, 52, 56, 60, 64, 68, 72, 76,
// 20 - 29
 80, 84, 88, 92, 96,100,104,108,112,116,
// 30 - 39
120,124,128,132,136,140,144,148,152,156,
// 40 - 49
160,164,168,172,176,180,184,188,192,196,
// 50 - 59
200,204,208,212,216,220,224,228,232,236
};
#endif


#ifdef RMS_CLOCK
const UInt8 const hourLookup[] = {
	00,		// 0 place holder
	17,		// 1
	36, 	// 2
	55, 	// 3
	75,		// 4
	95,		// 5
	118,	// 6
	138,	// 7
	158,	// 8
	180,	// 9
	200,	// 10
	220,	// 11
	240		// 12
};

const int const minuteLookup[] = {
// 0 - 9
  0,  3,  6,  9, 12,
 17, 20, 24, 28, 32,
// 10 - 19
 36, 39, 43, 47, 52,
 55, 58, 63, 66, 72,
// 20 - 29
 75, 79, 83, 87, 91, 
 95,100,105,110,114,
// 30 - 39
118,122,126,130,134,
138,142,146,150,154,
// 40 - 49
158,163,168,173,178,
180,184,188,192,196,
// 50 - 59
200,204,208,212,216,
220,224,228,232,236
};
#endif

DISABLE_SERIAL
DECLARE_INT_FLAGS;
DECLARE_TIMERS(3);

UInt8	tickTock;
UInt8	seconds;
UInt8	minutes;
UInt8	hours;
UInt8 	pm;

UInt8	mramp;
UInt8	hramp;
UInt8	rflag;
UInt8	sflag;
UInt8	sramp;
UInt8	rampDelay;

UInt8	cramp;
UInt8	cDest;

Int16	destVal;
Int16 	destInt;
UInt8	secFlag = 0;
UInt8	tickFlag = 0;
	
void
doRamp(){
	if ( --rampDelay )
		return;
	rampDelay = LED_RAMP_DELAY;
	if ( ++secFlag == 9 ) {
		secFlag = 0;
		if ( rflag ) {
			if ( hramp > LED_MIN )
				--hramp;
		} else {
			if ( mramp > LED_MIN )
				--mramp;
		}	
	} 
	if ( ++tickFlag == 2 ) {
		tickFlag = 0;
		if ( !sflag ) {
			if ( sramp > LED_MIN )
				--sramp;
		}
	}		

	if ( cramp != cDest ) {
		if ( cramp < cDest )
			++cramp;
		else
			--cramp;
	}
	writeD2A(HOUR_LED_CHANNEL,hramp);
	writeD2A(MINUTE_LED_CHANNEL,mramp);
	writeD2A(TICK_TOCK_CHANNEL,sramp);
	writeD2A(CLOCK_CHANNEL,cramp);
}
void
setHours() {
	cDest = hourLookup[hours];
	
}
void
setMinutes() {
	cDest = minuteLookup[minutes];
}

UInt8 ccount = 0;
void
doClock(){	
	if ( clockDisable )
		return;
	switch ( ccount ) {
		default:
		case 0:
			
			setHours();
			//destVal = hourLookup[hours];
			//destInt = destVal - minuteLookup[minutes];
			mramp = LED_MIN;
			hramp = LED_MAX;
			rflag = 1;

			sramp = LED_MAX;
			sflag = 0;
			break;
		case 1:
			sramp = LED_MAX;
			sflag = 0;
			break;
			
		case 2:
			setMinutes();
			//destVal = minuteLookup[minutes];
			//destInt = destVal - hourLookup[hours];
			mramp = LED_MAX;
			hramp = LED_MIN;
			rflag = 0;
			sramp = LED_MAX;
			sflag = 0;
			break;
		case 3:
			sramp = LED_MAX;
			sflag = 0;
			break;
	}
	if ( ++ccount == 4 )
		ccount = 0;
	doRamp();
}
void
versionInit(){
	
}
void
bumpMinutes(UInt8 hourFlag){}

void
updateClock() {
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



void
init()
{
	PORTB = 0xff;
	D2A_CLK	= 0;
	D2A_LOAD = 1;
	D2A_DATA = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	
	// 
	TRISB = 0x21; // RB5 - minutes RB0 - 60HZ  RB1,2,3 -D2A outputs
	TRISA = 0x01; // RA0 - hours
	PORTA = 0;
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 0;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	ADCON0 = 0;
	ADCON1 = 6;		// set all a/d ports to digital
	INTEDG = 1;
	tickTock = 0;
	seconds = 0;
	minutes = 0;
	hours = 1;
	pm = 0;
	PM_LIGHT = 0;
}


void
writeD2A(UInt8 addr, UInt8 data)
{
	UInt8 mask;
	for ( mask = 0x4; mask; mask >>= 1 )
	{
		D2A_CLK = 1;
		D2A_DATA = (addr & mask) ? 1 : 0;
		D2A_CLK = 0;
	}
	D2A_CLK  = 1;
	D2A_DATA = 0;	// hairy edge time delay at 20MHZ
	D2A_DATA = 0;
	D2A_DATA = 0;
	D2A_CLK  = 0;
	for ( mask = 0x80; mask; mask >>=1 )
	{
		D2A_CLK = 1;
		D2A_DATA = (data & mask) ? 1 : 0;
		D2A_CLK = 0;
	}
	D2A_LOAD = 0;
	D2A_LOAD = 1;
}

UInt8 	testminutes = 0;
UInt8   testhours = 0;


void
handleBut(UInt8 butVal)
{}

void
main()
{
	
	init();
	versionInit();
	initInterrupt(1,1);
	initButtons();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		RC5 = 0;
		if ( oneKhz )
		{
			if ( RC4 )
				RC4 = 0;
			else
				RC4 = 1;
			oneKhz = 0;
			checkButtons();
			doRamp();
		}
		
		if ( pinInt )
		{
			pinInt = 0;
			updateClock();
			doClock();
		}
	}
} 
