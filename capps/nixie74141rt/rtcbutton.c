#include <pic.h>
//#include <sys.h>
#include "x8types.h"
#include "rtcbutton.h"
#include "rtclock.h"
#include "nixie74141.h"
uint8_t 	lowerTimer;
uint8_t	upperTimer;
uint8_t 	modeTimer;
uint8_t 	lowCount;

#define FAST_TIME	75
#define SLOW_TIME	150



void
checkButtons() {
	if ( !LOWER_BUTTON ) {
		if ( !--lowerTimer ) {
			rtcStop();
			if ( lowCount < 5 ) {
				++lowCount;
				lowerTimer = SLOW_TIME;
			} else {
				lowerTimer = FAST_TIME;
			}
			rtcBumpMin();
			rtcClearSec();
			setSeconds();
			setMinutes();
			displaySeconds();
			displayMinutes();
		}
	} else {	
		lowCount = 0;
		lowerTimer = 1;
	} 

	if ( !UPPER_BUTTON ) {		
		if ( !--upperTimer ) {
			rtcStop();
			rtcBumpHour();
			upperTimer = SLOW_TIME;
			setHours();
			displayHours();
		}
	} else {	
		upperTimer = 1;
	}
}

void
initButtons() {
	LOWER_TRIS = 1;
	UPPER_TRIS = 1;
	MODE_TRIS = 1;
}
