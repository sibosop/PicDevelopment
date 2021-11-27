#ifndef MTCLK_H
#define MTCLK_H

//#define VERSION_1
//#define VERSION_2
//#define VERSION_3
#define VERSION_4
//#define VERSION_5
//#define VERSION_6
//#define VERSION_7
//#define VERSION_8
//#define VERSION_9
//#define VERSION_10

#include "timers.h"
#define HOUR_TIMER 0
#define MINUTE_TIMER 1
#define RAMP_TIMER 2

#define	TICK_TOCK_CHANNEL 0
#define SECONDS_CHANNEL 1
#define MINUTES_CHANNEL 2
#define HOURS_CHANNEL 3

extern  UInt16	tickTock;
extern  UInt16	seconds;
extern  UInt16	minutes;
extern  UInt16	hours;

void bumpHours();
void doClock();
void doRamp();
void versionInit();
void bumpMinutes(UInt8 hourFlag);

#define MILLIT	104

//#define	PERIOD		304
#define PERIOD	360
#define HIGH_SET	PERIOD

#define TICK_OFFSET	40
#define LOW_TICK	(TICK_OFFSET)
#define HIGH_TICK	(HIGH_SET-TICK_OFFSET)

#define APP_TIMER 	3
#define TT_DELAY	20
#define APP_DELAY	400


#define	TT_MIN	LOW_SET
#define TT_MAX	HIGH_SET
#define NUM_METERS	4
extern UInt16	duties[NUM_METERS];

extern void checkButtons();
extern void initButtons();

extern void setMinutes();
extern void setHours();
#endif
