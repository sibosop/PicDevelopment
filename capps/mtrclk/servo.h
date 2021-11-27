#ifndef SERVO_H
#define SERVO_H
#include <pic.h>
#include <sys.h>

#include "types.h"

#define SERVO_REFRESH	58
#define	FULL_CCW		2
#define FULL_CW			6
extern 	UInt8	refreshCnt;
extern	UInt8	dutyCnt;
extern	UInt8	servoPos;
extern 	UInt8	chimeOn;


void stopServo();
void startServo();
void startChime(UInt8);
void chimeTest();
#define USE_SERVO
#define	TEST_SERVO \
		if ( chimeOn){ \
			if ( dutyCnt && !(--dutyCnt) ){ \
				RB4 = 0; \
			}	\
			if ( !(--refreshCnt) ){ \
				refreshCnt = SERVO_REFRESH; \
				RB4 = 1; \
				dutyCnt = servoPos; \
			}	\
		} 
#endif
