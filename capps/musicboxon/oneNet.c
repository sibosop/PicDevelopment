#include <pic.h>
#include <sys.h>
#include "oneNet.h"

#ifdef MBMASTER
UInt16	onCounter;
UInt8	onMask;
#endif


int
checkOneNet() {
	int rval = 0;
#ifdef MBMASTER
	if ( ++onCounter == 5000 ) {
		onCounter = 0;
		onMask <<= 1;
		if ( onMask == 0x20 )
			onMask = 2;
		PORTA = onMask | 1;
	}
	if ( onMask == 0x10) 
		rval = 1;
#else
	rval = RA4;
#endif	

return rval;
}


void initOneNet(){
#ifdef MBMASTER
	TRISA = 0x00;
	onCounter = 0;
	onMask = 1;
#else
	TRISA = 0x10;
#endif
	
}






