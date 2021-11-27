#ifndef MTCLK_H
#define MTCLK_H

#define PERIOD	120
#define PERIOD_LOW	62
#define PERIOD_HIGH	(PERIOD_LOW + PERIOD)
#define INTERVAL	(PERIOD / 60)

extern void checkButtons();
extern void initButtons();
extern void initClock();
extern void doClock();
extern void updateClock();
extern void checkSpin();

extern void setMinutes();
extern void setHours();

extern UInt8	meterDuty;
extern UInt8	meterTimer;
extern UInt8	periodTimer;

extern UInt8	seconds;
extern UInt8	hours;
extern UInt8	minutes;
extern Boolean	clockDisable;

#endif
