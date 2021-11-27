#include "seg.h"
UInt8 segDotHi;
UInt8 segDotLo;
const UInt8 const segPattern[] = {
	0xFC, //0
	0x60,//1
	0xDA,//2
	0xF2,//3
	0x66,//4
	0xB6,//5
	0xBE,//6
	0xE0,//7
	0xFE,//8
	0xF6,//9
	0xEE,//A
	0x3E,//b
	0x9C,//C
	0x7A,//d
	0x9E,//e
	0x8E,//f
};
	
void
putNibble(UInt8 n, UInt8 d) {
	UInt8 pat = segPattern[n];
	UInt8 mask;
	pat |= d;
	for ( mask = 0x1; mask; mask <<= 1 ) {
		SEG_CLOCK = 0;
		if ( mask & pat )
			SEG_DATA = 1;
		else
			SEG_DATA = 0;
		SEG_CLOCK = 1; 
		SEG_CLOCK = 1;
	}
	SEG_CLOCK = 0;
	
}

void
segInit() {
	TRISB1 = 0;
	TRISB2 = 0;
	TRISB3 = 0;
	segDotHi = 0;
	segDotLo = 0;
}

void
segDisplay(UInt16 d) {
	putNibble(d & 0xf,segDotLo);
	putNibble((d >> 4) & 0xf,0);
	putNibble((d >> 8) & 0xf,segDotHi);
	putNibble(d >> 12,0);
	SEG_LATCH = 1;
	SEG_LATCH = 0;
}

