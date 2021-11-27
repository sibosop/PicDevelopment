#include <pic.h>
#include <sys.h>
#include "servo.h"

UInt16	refreshCnt;
bank1 UInt8	posCount[NUM_SERVOS];
bank1 UInt8	mask[NUM_SERVOS];
bank1 UInt8	counter[NUM_SERVOS];

void		
servoInt() {
	UInt8 sindex;
	for( sindex = 0; sindex < NUM_SERVOS; ++sindex ) {
		if ( counter[sindex] && (--counter[sindex] == 0)  ) {
			PORTA &= ~mask[sindex];
		} 
	}
	if ( ++refreshCnt > SERVO_REFRESH )  {	
		refreshCnt = 0;
		for(sindex=0; sindex < NUM_SERVOS; ++sindex ) {
			PORTA |= mask[sindex];
			counter[sindex] = posCount[sindex];
		}
	}
}

void		
initServo() {
	int	i;
	refreshCnt = 0;
	for(i=0;i<NUM_SERVOS;++i) {
		posCount[i] = 0;
		counter[i] = 0;
	}
	mask[0] = 1;
	mask[1] = 2;
	mask[2] = 4;
	mask[3] = 8;
	mask[4] = 0x20;
}

void	
moveServo(UInt8 servoNum, UInt8 pos) {
	posCount[servoNum] = pos;
}
