#ifndef CALLBACK_TIMER_H
#define CALLBACK_TIMER_H

#include "types.h"
extern void initCallbackTimers(void);
extern void checkCallbackTimers(void);
extern void timerCallback(UInt8 timer, void(*cb)(UInt8));
extern void cancelCallbackTimer(UInt8 timer);
extern void setCallbackTimer(UInt8 timer, UInt16 time);
extern void releaseCallbackTimer(UInt8 timer);
extern void suspendCallbackTimer(UInt8 timer);


#define CBANK bank1

typedef struct _callbackTimers {
	UInt16  value;
	UInt16	timer;
	UInt8	suspend;
	void	(*cb)(UInt8);
} CallbackTimers;

#define DECLARE_CALLBACK_TIMERS(x) \
	const UInt8	numTimers = (x); \
	CBANK CallbackTimers	callbackTimers[(x)]

extern const UInt8 numTimers;
extern CBANK CallbackTimers callbackTimers[];


#endif
