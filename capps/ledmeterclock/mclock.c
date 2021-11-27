

#include <pic.h>
#include <sys.h>

#include "types.h"
#include "mclock.h"
#define SER	RB1
#define LATCH	RB2
#define CLOCK	RB3
#define ENABLE	RB4

#define SPIN_SPEED 50
#define SPIN_DELAY	2000
UInt8 spinVal;
UInt8 spinEnd;
UInt8 spinCurrent;
UInt16 spinCount;
Boolean spinForward;
void
initClock() {
	meterDuty = 0;
	periodTimer = PERIOD_HIGH;
	meterTimer = 0;
	clockDisable = 0;
	spinEnd = 0;
	spinCurrent = 0;
	spinForward = 1;
	spinCount = SPIN_SPEED;
	TRISB1 = 0;
	TRISB2 = 0;
	TRISB3 = 0;
	LATCH = 0;
	CLOCK = 0;
	SER = 0;
	ENABLE = 0;
}


void
setLeds(UInt8 disp) {
	UInt16 sft = 1 << disp;
	UInt8	i;
	for (i = 0; i < 16; ++i ) {
		CLOCK = 0;
		if ( sft & 1 ) {
			SER = 1;
		} else {
			SER = 0;
		}
		CLOCK = 1;
		CLOCK = 1;
		sft >>= 1;
	}
	CLOCK = 0;
	LATCH = 1;
	LATCH = 0;
}


void
checkSpin() {
	if ( clockDisable ) {
		setLeds(hours);
		spinCount = SPIN_DELAY;
		return;
	}
	if ( !--spinCount ) {
		spinCount = SPIN_SPEED;
		setLeds(spinCurrent);
		if ( spinEnd == spinCurrent )
			return;
		if ( spinForward ) {
			spinCurrent = (spinCurrent + 1) % 12;
		} else {
			if ( !spinCurrent )
				spinCurrent = 11;
			else
				--spinCurrent;
		}		
	}
}

void
doClock() {
	meterDuty = PERIOD_LOW + (minutes * INTERVAL);
	spinEnd = hours % 12;
	if ( !spinForward ) {
		spinForward = 1;
		spinCurrent = (spinEnd + 1) % 12;
	} else {
		spinForward = 0;
		if ( !spinEnd ) 
			spinCurrent = 11;
		else
			spinCurrent = spinEnd - 1;
	}
}
void
updateClock() {
	if ( ++seconds == 60 ) {
		seconds = 0;
		if ( ++minutes == 60 ) {
			minutes = 0;
			if ( ++hours == 12 ) {
				hours = 0;
			}
		}
	}
}

void
setMinutes()
{}

void
setHours()
{}

