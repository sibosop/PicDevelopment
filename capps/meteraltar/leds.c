#include "types.h"
#include "leds.h"
#include "display.h"
#include "pic.h"

#define LED_DAT	RB1 /* Yellow */
#define LED_CLK	RB2 /* Green */

UInt8	numLeds;
bank1 Rgb ledBuf0[25];
bank2 Rgb ledBuf1[25];
bank3 Rgb ledBuf2[10];
Boolean changeFlag;

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

void
ledGet(UInt8 n, Rgb *v) {
	UInt8 b = n/25;
	n %= 25;
	switch (b) {
		default:
		case 0:
			*v = ledBuf0[n];
			break;
		case 1:
			*v = ledBuf1[n];
			break;

		case 2:
			*v = ledBuf2[n];
			break;
	}
}
void
ledCheck() {
	Rgb v;
	UInt8	i;
	if ( changeFlag ) {
		for (i = 0; i < numLeds; ++i ) {
			ledGet(i,&v);
			ledWriteByte(v.r);
			ledWriteByte(v.g);
			ledWriteByte(v.b);
		}
	}
	changeFlag = 0;
}

void 
ledSet(UInt8 n, Rgb *val) {
	UInt8 b = n/25;
	n %= 25;
	switch (b) {
		default:
		case 0:
			ledBuf0[n] = *val;
			break;
		case 1:
			ledBuf1[n] = *val;
			break;
		case 2:
			ledBuf2[n] = *val;
			break;
	}
	changeFlag = 1;
}


void
ledClear() {
	UInt8 i;
	Rgb zrgb;
	zrgb.r = zrgb.g = zrgb.b = 0;
	
	for ( i = 0; i < numLeds; ++i ) {
		ledSet(i,&zrgb);
		ledWriteByte(0);
		ledWriteByte(0);
		ledWriteByte(0);
	}
	ledCheck();
}


void
ledInit(UInt8 cnt) {
	numLeds = cnt;
	ledClear();
}
