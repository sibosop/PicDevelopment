#ifndef ButtonSpec_h
#define ButtonSpec_h
#define	HOUR_BUTTON	RA1
#define HOUR_TRIS	TRISA1
#define MINUTE_BUTTON	RA0
#define MINUTE_TRIS	TRISA0

#define FAST_TIME	150
#define SLOW_TIME	300

extern UInt8 seconds;
extern UInt8 minutes;
extern UInt8 hours;
extern Boolean clockDisable;
extern void setMinutes();
extern void setHours();
#endif
