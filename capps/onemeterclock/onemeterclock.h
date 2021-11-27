#ifndef ONEMETERCLK_H
#define ONEMETERCLK_H


#include "timers.h"
#define HOUR_TIMER 0
#define MINUTE_TIMER 1
#define RAMP_TIMER 2
#define	D2A_CLK	 RB1
#define D2A_LOAD RB2
#define D2A_DATA RB3


#define	TICK_TOCK_CHANNEL 3
#define MINUTE_LED_CHANNEL 2
#define HOUR_LED_CHANNEL 1
#define CLOCK_CHANNEL 0

#define PM_LIGHT	RB4
#define MINUTE_SET	RB5
#define HOUR_SET	RA0

extern  UInt8	tickTock;
extern  UInt8	seconds;
extern  UInt8	minutes;
extern  UInt8	hours;
extern  UInt8	pm;

void bumpHours();
void writeD2A(UInt8 addr,UInt8 data);
void doClock();
void doRamp();
void versionInit();
void bumpMinutes(UInt8 hourFlag);
void setMinutes();
void setHours();

#define BUTT_TIMER1 0
#define BUTT_TIMER2 1

#endif
