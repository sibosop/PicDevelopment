#include <pic.h>
#include <sys.h>
#include "types.h"
#include "rtcbutton.h"
#include "rtclock.h"

UInt8 	lowerTimer;
UInt8	upperTimer;
UInt8 	modeTimer;
UInt8 	lowCount;

extern void displayTime();

#define FAST_TIME	75
#define SLOW_TIME	150

void
checkSetMode() {
	setMode = PORTA & 0x7;
	switch ( setMode ) {
		default:
			runMode = DisplayNormal;
			break;

		case SetTime:
			runMode = DisplayTime;
			break;

		case SetDate:
			runMode = DisplayDate;
			break;
		case SetYear:
			runMode = DisplayYear;
			break;

		case SetWeekday:
			runMode = DisplayWeekday;
			break;

		case SetAlarm1:
			runMode = DisplayAlarm1;
			break;
		case SetAlarm2:
			runMode = DisplayAlarm2;
			break;
	}
}

void
checkButtons() {	
	UInt8 button = 0;
	if ( !MODE_BUTTON ) {
		if ( !--modeTimer ) {
			modeTimer = SLOW_TIME;
			setMode++;
			if ( setMode == LastMode ) {
				setMode = Running;
			}
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
					break;
			
				case SetDate:
					rtcBumpDate();
					break;

				case SetYear:
					rtcBumpYear();
					break;

				case SetWeekday:
					rtcBumpWeekday();
					break;

				default:
					break;
			}
			displayTime();
		}
	} else {	
		button += 1;
		lowCount = 0;
		lowerTimer = 1;
	} 

	if ( !UPPER_BUTTON ) {		
		if ( !--upperTimer ) {
			rtcStop();
			switch ( setMode ) {
				case SetTime:
					rtcBumpHour();
					break;
				case SetDate:
					rtcBumpMon();
					break;
				default:
					break;
			}
			displayTime();
			upperTimer = SLOW_TIME;
		}
	} else {	
		button += 1;
		upperTimer = 1;
	}
	if ( button == 2 ) 
		rtcRestart();
}

void
initButtons() {
	LOWER_TRIS = 1;
	UPPER_TRIS = 1;
	MODE_TRIS = 1;
}
