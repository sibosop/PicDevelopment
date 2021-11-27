#ifndef INTERRUPT_H
#define INTERRUPT_H
#include "types.h"

extern void initInterrupt(UInt8,UInt8);
extern Boolean pinInt;
extern Boolean	oneKhz;

// this declare saves a few bytes
// by putting all the flags in the
// same module
#define DECLARE_INT_FLAGS \
	Boolean pinInt; \
	Boolean oneKhz;

#define TWEEK_MILLI(mi,mit) \
	const UInt8 milliCount = mi; \
	const UInt8 millitCount = mit; 

extern const UInt8 milliCount;
extern const UInt8 millitCount;

#ifdef USE_TIMER_CALLBACK
void timerCallback(void);
#endif

#ifdef USE_PIN_CALLBACK
void pinCallback(void);
#endif




#endif
