

#include <pic.h>
#include <sys.h>
#include "types.h"
#include "nixie74141.h"

#define	HOUR_BUTTON	RA5
#define HOUR_TRIS	TRISA5
#define MINUTE_BUTTON	RA4
#define MINUTE_TRIS	TRISA4

#define FAST_TIME	75
#define SLOW_TIME	150

UInt16	minuteTimer;
UInt16 	hourTimer;
UInt8 minCount;
void
checkButtons() {
	if ( !--minuteTimer ) {
		if ( !MINUTE_BUTTON ) {
			seconds = 0;
			if ( ++minutes == 60 ) {
				minutes = 0;
			}
			setMinutes();
			if ( minCount < 5 ) {
				++minCount;
				minuteTimer = SLOW_TIME;
			} else {
				minuteTimer = FAST_TIME;
			}
		} else {
			minCount = 0;
			minuteTimer = SLOW_TIME;
		} 
	} 
	if ( !--hourTimer ) {
		if ( !HOUR_BUTTON ) {
			if ( ++hours == 13 ) 
				hours = 1;
			setHours();
		}
		hourTimer = SLOW_TIME;
	}
}

void
initButtons() {
	HOUR_TRIS = 1;
	MINUTE_TRIS = 1;
	hourTimer = SLOW_TIME;
	minuteTimer = SLOW_TIME;
}
