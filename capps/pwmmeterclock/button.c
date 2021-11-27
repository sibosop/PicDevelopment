

#include <pic.h>
#include <sys.h>
#include "types.h"
#include "mclock.h"

#define	HOUR_BUTTON	RA0
#define HOUR_TRIS	TRISA0
#define MINUTE_BUTTON	RB5
#define MINUTE_TRIS	TRISB5

#define FAST_TIME	1000
#define SLOW_TIME	5000

UInt16	minuteTimer;
UInt16 	hourTimer;
UInt8 minCount;
Boolean minDb;
UInt8 minDbCount;
Boolean hourDb;
UInt8 hourDbCount;
#define DEBOUNCE_COUNT	100


void
checkButtons() {
	if ( !MINUTE_BUTTON ) {
		if ( !minDb ) {
			minDb = 1;
			minDbCount = DEBOUNCE_COUNT;
			minuteTimer = 1;
			return;
		} else {
			if ( minDbCount && --minDbCount )
				return;
			if ( !--minuteTimer ) {
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
			}
		} 
	} else {
		minDb = 0;
		minuteTimer = SLOW_TIME;
		minCount = 0;
	}
	if ( !HOUR_BUTTON ) {
		if ( !hourDb ) {
			hourDb = 1;
			hourDbCount = DEBOUNCE_COUNT;
			hourTimer = 1;
			return;
		} else {
			if ( hourDbCount && --hourDbCount )
				return;
			if ( !--hourTimer ) {
				if ( ++hours == 13 ) {
					hours = 1;
				}
				setHours();
				hourTimer = SLOW_TIME;
			}
		}
	} else {
		hourDb = 0;
	}
}

void
initButtons() {
	HOUR_TRIS = 1;
	MINUTE_TRIS = 1;
	hourTimer = 1;
	minuteTimer = 1;
	hourDb = 0;
	minDb = 0;
}
