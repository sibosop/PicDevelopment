#ifndef STEPPER_H
#define STEPPER_H

#include "types.h"

void initSteppers(void);
void checkSteppers(void);
void stepperSpeed(UInt8,UInt16);
void stepperDir(UInt8,UInt8);
void stepperRevCallback(UInt8,void(*cb)(UInt8));
void stepperSetRevTrigger(UInt8,UInt16);
void stepperCancelTrigger(UInt8);
#ifndef NUM_MOTORS
#define NUM_MOTORS 2
#endif
#define CLOCKWISE	1
#define COUNTER_CLOCKWISE 0;
#endif
#ifndef STEPS_PER_REV
#define STEPS_PER_REV	100
#endif

