#ifndef MTCLK_H
#define MTCLK_H

#define VERSION_1
//#define VERSION_2
//#define VERSION_3
//#define VERSION_4
//#define VERSION_5
//#define VERSION_6
//#define VERSION_7
//#define VERSION_8
//#define VERSION_9
//#define VERSION_10


#define	TICK_TOCK_CHANNEL 0
#define MINUTE_LED_CHANNEL 1
#define HOUR_LED_CHANNEL 2
#define CLOCK_CHANNEL 3

extern  UInt8	tickTock;
extern  UInt8	seconds;
extern  UInt8	minutes;
extern  UInt8	hours;

void doClock();
void doRamp();
void updateClock();
void versionInit();

//#define	PERIOD		304
#define PERIOD	720
#define HIGH_SET	PERIOD






#define NUM_METERS	4
extern UInt16	duties[NUM_METERS];

extern void checkButtons();
extern void initButtons();

extern void setMinutes();
extern void setHours();
extern void bumpTime();

extern UInt16	mramp;
extern UInt16	hramp;
extern Boolean	rflag;
extern UInt16	sramp;
#endif
