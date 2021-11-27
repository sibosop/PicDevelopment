#include "stepper.h"
void stepOn(UInt8 m, UInt8 s);
void stepOff(UInt8 m, UInt8 s);
void stepStop(UInt8 m);
typedef struct _MotorState {
	UInt8	stepCnt;
	UInt8	dir;
	UInt8	id;
	UInt16	speed;
	UInt16	timer;
	UInt16	numSteps;
	UInt16	revCnt;
	UInt16 	revTrigger;
	void	(*revCallback)(UInt8);
} MotorState;

bank3 MotorState motors[NUM_MOTORS];

void
stepperSpeed(UInt8 m,UInt16 speed) {
	if ( speed == 0 )
		stepStop(m);
	motors[m].speed = speed;
}

void
stepperDir(UInt8 m, UInt8 dir ) {
	motors[m].dir = dir == CLOCKWISE ? CLOCKWISE : COUNTER_CLOCKWISE;
}

void 
stepperRevCallback(UInt8 m,void(*cb)(UInt8)) {
	motors[m].revCallback= cb;
}

void
stepperSetRevTrigger(UInt8 m, UInt16 t) {
	motors[m].revTrigger = t;
}

void
stepperCancelTrigger(UInt8 m) {
	motors[m].revTrigger = 0;
}





void
initSteppers(void) {
	UInt8 m;
	for ( m = 0; m < NUM_MOTORS; ++m ) {
		bank3 MotorState *mp = &motors[m];
		mp->stepCnt = 0;
		mp->dir = CLOCKWISE;
		mp->speed = 0;
		mp->timer = 0;
		mp->id = m;
		mp->numSteps = 0;
		mp->revCnt = 0;
		mp->revTrigger = 0;
		mp->revCallback = 0;
	}
}



void
checkSteppers(void)
{
	UInt8	m;
	bank3	MotorState *mp;
	for ( m = 0; m < NUM_MOTORS; ++m ) {
		mp = &motors[m];
		if ( !mp->speed )
			continue;

		if ( mp->timer ) {
			--mp->timer;
		} else {
			mp->timer = mp->speed;
			stepOff(mp->id,mp->stepCnt);
			if ( mp->dir == CLOCKWISE )
			{
				++(mp->stepCnt);
				if ( mp->stepCnt ==4 )
					mp->stepCnt = 0;
				stepOn(mp->id,mp->stepCnt);
			}
			else
			{
				--(mp->stepCnt);
				if ( mp->stepCnt == 255 )
					mp->stepCnt = 3;
				stepOn(mp->id,mp->stepCnt);
			}
			if ( mp->revTrigger ) {
				mp->numSteps++;
				if ( mp->numSteps >= STEPS_PER_REV ) {
					mp->numSteps = 0;
					mp->revCnt++;
					if ( mp->revCnt >= mp->revTrigger ) {
						mp->revCallback(mp->id);
						mp->revCnt = 0;
					}
				}
			}
		}
	}
}
