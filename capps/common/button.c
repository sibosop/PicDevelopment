#include <pic.h>
#include <sys.h>
#include "types.h"
#include "button_spec.h"

UInt16	minuteTimer;
UInt16 	hourTimer;
UInt8 minCount;
void setMinutes();
void setHours();
Boolean minDisable;
Boolean hrDisable;
//Boolean clockDisable;
void
checkButtons() {	
	if ( !MINUTE_BUTTON ) {
		if ( !--minuteTimer ) {
			minDisable = 1;
			seconds = 0;
			if ( ++minutes == 60 ) {
				minutes = 0;
			}
			if ( minCount < 5 ) {
				++minCount;
				minuteTimer = SLOW_TIME;
			} else {
				minuteTimer = FAST_TIME;
			}
			setMinutes();
		}
	} else {
		minDisable = 0; 
		minCount = 0;
		minuteTimer = 1;
	} 


	if ( !HOUR_BUTTON ) {
		if ( !--hourTimer ) {
			hrDisable = 1;
			if ( ++hours == 13 ) 
				hours = 1;
			setHours();
			hourTimer = SLOW_TIME;
		}
	} else {
		hrDisable = 0;
		hourTimer = 1;
	}
	
	
	if ( hrDisable || minDisable )
		clockDisable = 1;
	else
		clockDisable = 0;
}

void
initButtons() {
	HOUR_TRIS = 1;
	MINUTE_TRIS = 1;
	hourTimer = 1;
	minuteTimer = 1;
	minDisable = 0;
	hrDisable = 0;
}
