#ifndef ButtonSpec_h
#define ButtonSpec_h
#define	HOUR_BUTTON	RB5
#define HOUR_TRIS	TRISB5
#define MINUTE_BUTTON	RA0
#define MINUTE_TRIS	TRISA0

#define FAST_TIME	20
#define SLOW_TIME	200

extern UInt8 seconds;
extern UInt8 minutes;
extern UInt8 hours;
extern Boolean clockDisable;
extern void setMinutes();
extern void setHours();
#endif
