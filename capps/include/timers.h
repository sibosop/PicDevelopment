#ifndef TIMERS_H
#define TIMERS_H

#include "types.h"
extern void initTimers(void);
extern void checkTimers(void);
extern UInt8 timerReady(UInt8 timer);
extern void setTimer(UInt8 timer, UInt16 time);

#if defined(_16F876)
#define TIMER_BANK	bank1
#elif defined(_16F877A)
#define TIMER_BANK	bank1
#else
#define TIMER_BANK
#endif



#define DECLARE_TIMERS(x) \
	const UInt8	numTimers = (x); \
	TIMER_BANK UInt16	Timers[(x)]

extern const UInt8 numTimers;
extern TIMER_BANK UInt16 Timers[];

#endif
