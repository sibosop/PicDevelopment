#ifndef SERVO_H
#define SERVO_H
#include "types.h"
#define SERVO_REFRESH	360
#define	FULL_LEFT		10
#define FULL_RIGHT		FULL_LEFT+32
#define	START_SERVO		2
#define NUM_SERVOS		5

extern void		servoInt();
extern void		initServo();
extern void		moveServo(UInt8 servoNum, UInt8 pos);



#endif
