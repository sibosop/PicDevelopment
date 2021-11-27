#include <pic.h>
#include <sys.h>
#include "types.h"
#include "oneWireTemp.h"

#define owDelay(x){UInt8 d=x; while (--d);}
UInt16	owTimer = 0;
#define OW_TIME	1000	
void initOneWireTemp() {
	ONE_WIRE_TRIS = 0;
	ONE_WIRE_DEBUG_TRIS = 0;
	owTimer = OW_TIME;
}

// generate a reset delay
// this one is 520us with a 20 
// meg clock
#define resetDelay() \
	owDelay(0xff); \
	owDelay(0xff); \
	owDelay(0xff); \
	owDelay(100)


// micro second delays used by one wire
// these are 20 meg clock delays
#define delay20() owDelay(30)
#define delay10() owDelay(15)
#define delay60() owDelay(99)
void owReset() {
	ONE_WIRE_PORT = 0;
	ONE_WIRE_TRIS = 0;
	resetDelay();
	ONE_WIRE_TRIS = 1;
	while( ONE_WIRE_PORT == 1 )
		;
	resetDelay();
}

void owWrite(UInt8 data) {
	UInt8 mask;
	ONE_WIRE_TRIS = 0;
	for (mask=1; mask; mask <<=1 ) {
		ONE_WIRE_PORT = 0;
		if ( data & mask ) {
			owDelay(1);
			ONE_WIRE_PORT = 1;
		}
		delay60();
		ONE_WIRE_PORT = 1;
		owDelay(2);
	} 
}

UInt8
owRead() {
	UInt8 rval = 1;
	UInt8 mask;
	for ( mask = 1; mask; mask <<= 1 ) {
		ONE_WIRE_DEBUG=1;
		ONE_WIRE_PORT = 0;
		ONE_WIRE_TRIS = 0;
		owDelay(4);
		ONE_WIRE_TRIS = 1;	
		owDelay(2);
		rval |= ONE_WIRE_PORT ? mask : 0;
		ONE_WIRE_DEBUG=0;
		delay60();	
	}
	return rval;
}
UInt16 cval = 0;
UInt16 fval = 0;
void checkOneWireTemp() {
	double ftemp,ctemp;
	UInt8 busy=0;
	UInt16 tmp1;
	UInt16 tmp2;
	
	if ( --owTimer ) {
		ONE_WIRE_DEBUG = 0;
		return;
	}
	owTimer = OW_TIME;
	di();
	owReset();
	
	owWrite(0xCC);	//skip rom
	owWrite(0x44);	//start temp conversion
	ONE_WIRE_DEBUG=1;
	while ( !busy ) {
		busy = owRead();
	}
	owReset();
	owWrite(0xCC);
	owWrite(0xBE);
	tmp1 = owRead();
	tmp2 = owRead();
	cval = tmp1 | (tmp2 << 8);
	ctemp = cval;
	ctemp /= 16.0;
	ftemp = ((ctemp * (9.0/5.0)) + 32) * 10.0;
	ctemp *= 10.0;
	fval = (UInt16)ftemp;
	cval = (UInt16)ctemp;
	ei();
}
