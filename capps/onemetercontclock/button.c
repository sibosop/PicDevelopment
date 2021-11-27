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
//Boolean minDisable;
//Boolean hrDisable;
void
checkButtons() {
#if 0
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
#endif 
	if ( !--hourTimer ) {
		if ( !HOUR_BUTTON ) {
#if 0
			hrDisable = 1;
			if ( ++hours == 13 ) 
				hours = 1;
			setHours();
#endif
			bumpTime();
			hourTimer = SLOW_TIME;
		} else {
			//hrDisable = 0;
			hourTimer = 1;
		}
		
	}
#if 0
	if ( hrDisable || minDisable )
		clockDisable = 1;
	else
		clockDisable = 0;
#endif
}

void
initButtons() {
	HOUR_TRIS = 1;
	MINUTE_TRIS = 1;
	hourTimer = 1;
	minuteTimer = 1;
//	minDisable = 0;
//	hrDisable = 0;
}
