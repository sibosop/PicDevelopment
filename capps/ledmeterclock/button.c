#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"
#include "button_spec.h"


UInt16	minuteTimer;
UInt16 	hourTimer;
UInt8 minCount;
extern Boolean clockDisable;
extern void setMinutes();
extern void setHours();
Boolean minDisable;
Boolean hrDisable;
void
checkButtons() {
	if ( !--minuteTimer ) {
		if ( !MINUTE_BUTTON ) {
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
		} else {
			minDisable = 0; 
			minCount = 0;
			minuteTimer = 1;
		} 
	} 
	if ( !--hourTimer ) {
		if ( !HOUR_BUTTON ) {
			hrDisable = 1;
			if ( ++hours == 12 ) 
				hours = 0;
			setHours();
			hourTimer = SLOW_TIME;
		} else {
			hrDisable = 0;
			hourTimer = 1;
		}
		
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
