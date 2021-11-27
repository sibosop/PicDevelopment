#include "types.h"
#include "leds.h"
#include "display.h"
#include "pic.h"
#include "pseudorand.h"

#define LED_DAT	RA0 /* Yellow */ 
#define LED_CLK	RA1 /* Green */  

void
ledWriteByte(UInt8 b) {
	UInt8 mask = 0x80;
	UInt8 i;
	LED_CLK = 0;
	for ( i = 0; i < 8; ++i ) {
		LED_DAT = (mask & b) ? 1 : 0;
		LED_CLK = 1;
		LED_CLK = 0;
		mask >>= 1;
	}
	LED_CLK = 0;
}

typedef struct _oneColor {
	UInt8	current;
	UInt8	goal;
	UInt8	speed;
	UInt8	speedCnt;
} OneColor;

OneColor colors[NUM_LEDS * 3];



UInt16 ledTimer;
Boolean on;

void
ledInit() {
	UInt8 i;
	ledTimer = 0;
	on = 0;
	for ( i = 0; i < (NUM_LEDS * 3); ++i ) {
		colors[i].current = 0;
		colors[i].goal = (UInt8)pseudoRand(0,256);
		colors[i].speed = (UInt8)pseudoRand(MIN_LED_SPEED
							,MAX_LED_SPEED);
		colors[i].speedCnt = 0;
	}
}

void
ledDumpColors(void) {
	UInt8 i;
	for(i = 0; i < (NUM_LEDS * 3); ++i ) {
		ledWriteByte(colors[i].current);
	}
}

void
checkLeds() {
	UInt8 i;
	UInt8 change = 0;

	if ( ++ledTimer < LED_INTERVAL ) {
		return;
	}
	ledTimer = 0;

	for ( i = 0; i < (NUM_LEDS * 3); ++i ) {
		if ( ++colors[i].speedCnt == colors[i].speed ) {
			if ( colors[i].current == colors[i].goal ) {
				colors[i].goal = (UInt8)pseudoRand(0,256);;
				colors[i].speed = (UInt8)pseudoRand(MIN_LED_SPEED,MAX_LED_SPEED);
				colors[i].speedCnt = 0;
			} else if ( colors[i].current < colors[i].goal ) {
				++colors[i].current;
				change = 1;
			} else {
				--colors[i].current;
				change = 1;
			}
			colors[i].speedCnt = 0;
		}
	}
	if ( change )
		ledDumpColors();
}
