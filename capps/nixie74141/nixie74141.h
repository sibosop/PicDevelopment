#ifndef nixie74141_h
#define nixie74141_h

#define NUM_METERS	2

#define PERIOD	360
#define HIGH_SET	360
#define TICK_OFFSET	40
#define LOW_TICK	(TICK_OFFSET)
#define HIGH_TICK	(HIGH_SET-TICK_OFFSET)

#define NIXIE_HOUR_MSB	3
#define NIXIE_HOUR_LSB  2
#define NIXIE_MIN_MSB	1
#define NIXIE_MIN_LSB	0
extern UInt16 duty0;
extern UInt16 duty1;
extern UInt8 nixieVals[4];

#define TT_DELAY	3
#define VERSION_1
void doClock();
void doRamp();
void versionInit();
void checkButtons();
void initButtons();

extern UInt8	hours;
extern UInt8	minutes;
extern UInt8	seconds;
extern Boolean	hourFlag;
extern void setHours();
extern void setMinutes();
#endif
