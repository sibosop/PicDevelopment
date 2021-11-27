
#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "stepper.h"
#include "pseudorand.h"
#define NUM_RAMPS	2

typedef struct _ramp {
	UInt16 dest;
	UInt16 current;
	UInt16 incr;
	UInt8  dir;
	void (*callback)(UInt8,UInt16);
} Ramp;

bank3 Ramp ramps[NUM_RAMPS];

void
initRamps() {
	UInt8 i;
	for (i = 0;i<NUM_RAMPS;++i ) {
		ramps[i].dest = 0;
		ramps[i].callback = 0;
		ramps[i].current = 0;
		ramps[i].incr = 0;
		ramps[i].dir = 0;	
	}
}

void
setRampCallback(UInt8 r, void(*cb)(UInt8,UInt16)) {
	ramps[r].callback = cb;
}

void
checkRamps() {
	UInt8 i;
	bank3 Ramp *r;
	for (i = 0; i < NUM_RAMPS; ++i ) {
		r = &ramps[i];
		if ( r->current == r->dest )
			continue;
		if ( r->dir ) {
			if ( (r->current + r->incr) >= r->dest ) {
				r->current = r->dest;
			} else {
				r->current += r->incr;
			}
		} else {
			if ( (r->current - r->incr) <= r->dest ) {
				r->current = r->dest;
			} else {
				r->current -= r->incr;
			}
		}
		r->callback(i,r->current);
	}
}


void
startRamp(UInt8 r, UInt16 dest, UInt16 speed) {	
	if ( dest == ramps[r].current ) 
		return;
	
	ramps[r].dest = dest;
	ramps[r].dir = ramps[r].current < dest;
	if ( ramps[r].dir )
		ramps[r].incr = speed/(dest - ramps[r].current);
	else
		ramps[r].incr = speed/(ramps[r].current - dest);
}


DECLARE_INT_FLAGS;
__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);
void
init()
{
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	ADCON1 = 0x07;	// make register A all digital inputs.
	TRISA = 0x6;	// motor switch inputs on RA1 & RA2
	PORTA = 0;

	PORTB = 0;
	PORTC = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	INTEDG = 1;
	TMR1IE = 1;
	T1CON = 1;  // timer one enabled internal interrupt
	TMR1H = 0x80;
	TMR1L = 0;
}

#if 0
UInt8 s = 1;
UInt16 c =0;
UInt16 w = 5000;
UInt8 d = 1;
#endif
typedef struct _Song {
	UInt8	note;
	UInt8	len;
} Song;


const Song song[] = {
	40,2,
	33,4,
	30,1,
	26,4,
	24,1,
	26,1,
	30,4,
	35,2,
	45,4,
	40,1,
	35,1,
	33,4,
	40,2,
	40,4,
	42,1,
	40,1,
	36,4,
	42,2,
	54,4,
	40,2,
	33,4,
	30,2,
	26,4,
	24,1,
	26,1,
	30,4,
	35,2,
	45,4,
	40,1,
	35,1,
	33,4,
	35,1,
	40,1,
	42,4,
	45,1,
	42,1,
	40,4,
	40,8,
	0,0
};

typedef struct _motorState {
	UInt16	beat;
	UInt8	wait;
	UInt8	pos;
} MotorState;
MotorState	ms[2];

const UInt16 tempo[2] = {
	3000,3800
};
main() {
	UInt8	i = 0;
	ms[0].beat = 0;
	ms[0].wait = 0;
	ms[0].pos = 0;
	ms[1].beat = 0;
	ms[1].wait = 0;
	ms[1].pos = 0;
	init();
	initSteppers();
	initInterrupt(0,1);

	//initRamps();
	ei();
	stepperDir(0,0);
	stepperDir(1,1);
	stepperSpeed(0,40);
	stepperSpeed(1,10);
	//setRampCallback(0,stepperSpeed);
	//setRampCallback(1,stepperSpeed);
	while ( 1 ) {
		CLRWDT();
		if ( oneKhz ){
			oneKhz = 0;
			checkSteppers();
			for ( i = 0; i < 2; ++i ) {
				if ( ++ms[i].beat == tempo[i] ) {
					ms[i].beat = 0;
					if ( !song[ms[i].pos].note ) {
						ms[i].pos = 0;
						ms[i].wait = 0;
					}
					stepperSpeed(i,song[ms[i].pos].note);
					if ( ++ms[i].wait == song[ms[i].pos].len ) {
						ms[i].pos++;
						ms[i].wait = 0;
					}
				}
			}
#if 0
			checkRamps();
			if ( ++c == w ) {
				if ( s == d ) {
					d = pseudoRand(3,16);
				} else if ( s < d ) {
					if ( ++s == d ) {
						d = pseudoRand(3,16);
					}
				} else {
					if ( --s == d ) {
						d = pseudoRand(3,16);
					}
				}
				stepperSpeed(1,s);
				//w = pseudoRand(100,500);
				w = 200 ;
				c = 0;

			}	
#endif
		}	
	}	
}
