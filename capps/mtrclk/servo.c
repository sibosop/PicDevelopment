#if defined(VERSION_5)

#include "servo.h"

UInt8	refreshCnt;
UInt8	dutyCnt;
UInt8	servoPos;
UInt8	servoTimer;
UInt8	chimeCnt;
UInt8	numChimes;
UInt8	chimeOn;
#define	CHIME_DELAY	2

void stopServo() {
	refreshCnt = 0;
	dutyCnt = 0;
	RB4	= 0;
	chimeOn = 0;
}

void startServo()
{
	refreshCnt = 1;
	RB4 = 1;
	chimeOn = 1;
}

void startChime(UInt8 num)
{
	chimeCnt = 1;
	numChimes = num;
	startServo();
}


void chimeTest() {
	if ( !numChimes ){
		if ( servoPos )
			stopServo();
		return;
	}

	if ( !(--chimeCnt) ) {
		chimeCnt = 2;
		servoPos = (servoPos == FULL_CCW) ? FULL_CW : FULL_CCW;	
		--numChimes;	
	}
}
#endif
