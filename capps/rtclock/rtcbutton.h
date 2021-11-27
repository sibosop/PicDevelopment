#ifndef RTC_BUTTON_H
#define RTC_BUTTON_H

#define LOWER_BUTTON RA3
#define LOWER_TRIS   TRISA3
#define UPPER_BUTTON RA4
#define UPPER_TRIS   TRISA4

#define MODE_BUTTON	RA2
#define MODE_TRIS	TRISA2
void checkButtons();
void initButtons();
void checkSetMode();

typedef enum _smodes_ {
	Running = 0
	,SetTime
	,SetDate
	,SetYear
	,SetWeekday
	,SetAlarm1
	,SetAlarm2
	,LastMode
} SetMode;

typedef enum _rmodes_ {
	DisplayNormal
	,DisplayTime
	,DisplayDate
	,DisplayYear
	,DisplayWeekday
	,DisplayAlarm1
	,DisplayAlarm2
} RunMode;

extern RunMode runMode;
extern SetMode setMode;
#endif
