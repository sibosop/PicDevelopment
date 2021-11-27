#include "display.h"


const char * const bvals = "0123456789ABCDEF";
/*
** serial display interface jameco 2118651
*/
void
dspCmd() {
	writeChar(0xFE);
}

void 
displayOn(UInt8 on) {
	dspCmd();
	if ( on ) {
		writeChar(0x41);
	} else {	
		writeChar(0x42);
	}
}

void 
dspBlink(UInt8 on) {
	dspCmd();
	if ( on ) 
		writeChar(0x4b);
	else
		writeChar(0x4c);
}		

void
dspUnderline(UInt8 on) {
	dspCmd();
	if ( on ) 
		writeChar(0x47);
	else
		writeChar(0x48);
}

void
dspCursorHome() {
	dspCmd();
	writeChar(0x46);
}
void
dspChar(UInt8 c) {
	writeChar(c);
}

void
dspBrightness(UInt8 b) {
	dspCmd();
	writeChar(0x53);
	writeChar(b);
}

void
dspContrast(UInt8 c) {
	dspCmd();
	writeChar(0x52);
	writeChar(c);
}

void
dspSetCursor(UInt8 x, UInt8 y) {
	UInt8 pos = x;
	if ( y ) 
		pos += 0x40;

	dspCmd();
	writeChar(0x45);
	writeChar(pos);
}

void 
dspCharAt(UInt8 c, UInt8 x, UInt8 y) {
	dspSetCursor(x,y);
	dspChar(c);
}	

void
dspClearScreen() {
	dspCmd();
	writeChar(0x51);
}
void
dspByte(UInt8 b) {
	UInt8 nib = b >> 4;
	writeChar(bvals[nib]);
	writeChar(bvals[b & 0xf]);
}

void
dspString(const char *s) {
	for( ;*s;++s ) 
		writeChar(*s);
}
		
	
void
initDisplay() {
	displayOn(1);
	dspCursorHome();
	dspUnderline(0);
	dspBlink(0);
	dspBrightness(3);
	dspContrast(40);
	dspClearScreen();
}

