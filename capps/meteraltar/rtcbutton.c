#include <pic.h>
#include <sys.h>
#include "types.h"
#include "rtcbutton.h"
#include "rtclock.h"
#include "meteraltar.h"
#include "display.h"


UInt8 	lowerTimer;
UInt8	upperTimer;
UInt8 	modeTimer;
UInt8 	lowCount;

extern void displayTime();

#define FAST_TIME	75
#define SLOW_TIME	150


SetMode	setMode;
#define DSP_MODE_POS	11



void dspMode(void) {
	dspSetCursor(DSP_MODE_POS,0);
	switch (setMode) {
		default:
		case Running:
			setNoColor();
			dspString("Run ");
			break;

		case SetTime:
			setNoColor();
			dspString("Time");
			break;

		case SetHourColor:
			dspString("Hour");
			break;

		case SetMinuteColor:
			dspString("Min ");
			break;

		case SetSecondColor:
			dspString("Sec ");
			break;

	}
}



void
checkButtons() {	
	if ( !MODE_BUTTON ) {
		if ( !--modeTimer ) {
			modeTimer = SLOW_TIME;
			setMode++;
			
			if ( setMode == LastMode ) {
				setMode = Running;
			}
			dspMode();
		}
	}
	if ( setMode == Running )
		return;

	if ( !LOWER_BUTTON ) {
		if ( !--lowerTimer ) {
			rtcStop();
			if ( lowCount < 5 ) {
				++lowCount;
				lowerTimer = SLOW_TIME;
			} else {
				lowerTimer = FAST_TIME;
			}
			switch( setMode ) {
				case SetTime:
					rtcBumpMin();
					rtcClearSec();
					setSeconds();
					setMinutes();
					displayTime();
					break;
			
				case SetHourColor:
					setHourColor();
					break;

				case SetMinuteColor:
					setMinuteColor();
					break;

				case SetSecondColor:
					setSecondColor();
					break;

				default:
					break;
			}
		}
	} else {	
		lowCount = 0;
		lowerTimer = 1;
	} 

	if ( !UPPER_BUTTON ) {		
		if ( !--upperTimer ) {
			rtcStop();
			switch ( setMode ) {
				case SetTime:
					rtcBumpHour();
					setHours();
					displayTime();
					break;
				default:
					break;
			}
			displayTime();
			upperTimer = SLOW_TIME;
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
	setMode = Running;
	dspMode();
}
