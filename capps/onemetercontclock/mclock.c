

#include <pic.h>
#include <sys.h>

#include "types.h"
#include "mclock.h"

#define RMS_CLOCK


#define LED_RAMP_DELAY 3



#define LED_MIN	0
#define LED_MAX 180

UInt8	clockDest;
UInt8	cramp;
UInt8	tickTock;
UInt8	seconds;
UInt8	minutes;
UInt8	hours;
UInt16	time;
UInt8 	pm;
//Boolean clockDisable;
UInt16	rampTimer;

//Int16	destVal;
//Int16 	destInt;
UInt8	secFlag = 0;
	
UInt16 rampTimer;
void
doRamp(){
#if 0
	if ( --rampTimer ) 
		return;
//	RC4 = RC4 ? 0 : 1;
	rampTimer = LED_RAMP_DELAY;
	if ( sramp > LED_MIN )
			--sramp;
	if ( ++secFlag == 2 ) {
		secFlag = 0;
		if ( rflag ) {
			if ( hramp > LED_MIN )
				--hramp;
		} else {
			if ( mramp > LED_MIN )
				--mramp;
		}	
	} 
	if ( cramp != clockDest ) {
		if ( cramp < clockDest )
			++cramp;
		else
			--cramp;
	}
	duties[HOUR_LED_CHANNEL] = hramp;
	duties[MINUTE_LED_CHANNEL] = mramp;
	duties[TICK_TOCK_CHANNEL] = sramp;
	duties[CLOCK_CHANNEL] = cramp;
#endif
}

void
setHours() {
	clockDest = hours * 60;
	//duties[CLOCK_CHANNEL] = hours * 15 ;
}

void
setMinutes() {
	clockDest = minutes * 12;
	//duties[CLOCK_CHANNEL] = minutes * 3;
}

void
bumpTime() {
	++time;
	if ( time == PERIOD )
		time = 0;
	duties[CLOCK_CHANNEL] = time;
}


UInt8 ccount = 0;
void
doClock(){	
//	if ( clockDisable )
//		return;
	sramp = LED_MAX;
	switch ( ccount ) {
		default:
			break;
		case 0:
			setHours();
			mramp = LED_MIN;
			hramp = LED_MAX;
			rflag = 1;
			break;
			
		case 2:
			setMinutes();
			mramp = LED_MAX;
			hramp = LED_MIN;
			rflag = 0;
			break;
	}
	if ( ++ccount == 4 )
		ccount = 0;
	doRamp();
}
void
versionInit(){
	rampTimer = 1;
	hours = 1;
	minutes = 0;
	seconds = 0;
//	clockDisable = 0;
}
void
bumpMinutes(UInt8 hourFlag){}

void
updateClock() {
	if ( ++seconds == 60 ) {
		seconds = 0;
		bumpTime();
#if 0
		if ( ++minutes == 60 ) {
			minutes = 0;
			if ( ++hours == 13 ) {
				hours = 1;
			}
		}
#endif
	}
}
