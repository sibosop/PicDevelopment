#ifndef RTC_BUTTON_H
#define RTC_BUTTON_H

#define LOWER_BUTTON RB4
#define LOWER_TRIS   TRISB4
#define UPPER_BUTTON RB5
#define UPPER_TRIS   TRISB5

#define MODE_BUTTON	RB3
#define MODE_TRIS	TRISB3
void checkButtons();
void initButtons();
void checkSetMode();
void setHourColor();
void setMinuteColor();
void setSecondColor();

typedef enum _smodes_ {
	Running = 0
	,SetTime
	,SetHourColor
	,SetMinuteColor
	,SetSecondColor
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
