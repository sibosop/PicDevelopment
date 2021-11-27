#ifndef rt_clock_h
#define rt_clock_h
#include <pic.h>
#include <sys.h>
#include "types.h"
#define	RTC_CHIP_SELECT	RC7
#define RTC_MSO_DIR	TRISC5
#define RTC_12_HOUR_BIT	0x40
#define RTC_AM_PM_BIT	0x20
#define CLOCK_BANK_SIZE	16
void rtcInit();
void rtcWrite(UInt8 data, UInt8 addr);
UInt8 rtcRead(UInt8 addr);
void rtcInterrupt();
void rtcStart();
UInt8	rtcVal(UInt8 i);
extern UInt8	clockBankIndex;
extern UInt16	secCount;
extern bank3 UInt8 clockBank[CLOCK_BANK_SIZE];
extern UInt8 clockBankIndex;
extern Boolean	addrFlag;	
extern Boolean stopFlag;

UInt8	rtcGetMin();
UInt8	rtcGetHour();
UInt8	rtcGetWeekday();
UInt8	rtcGetMon();
UInt8	rtcGetDate();
UInt8	rtcGetCentury();
UInt8	rtcGetYear();
UInt8	rtcIsPm();
void	rtcStop();
void	rtcRestart();
void	rtcBumpMin();
void	rtcBumpHour();
UInt8	rtcGetSecond();
void	rtcBumpYear();
void	rtcBumpDate();
void	rtcBumpMon();
void	rtcBumpWeekday();
UInt8	bcd2Bin(UInt8);
#define	RTC_YEAR_REG	6
#define RTC_MON_REG		5
#define RTC_DATE_REG	4
#define RTC_WEEKDAY_REG	3
#define RTC_HOUR_REG	2
#define RTC_MIN_REG		1
#define	RTC_SEC_REG		0

#define rtcInterrupt() \
	if ( addrFlag ) { \
		addrFlag = 0;\
		dummy = SSPBUF;\
		SSPBUF = clockBankIndex;\
	} else {\
		clockBank[clockBankIndex] = SSPBUF;\
		if ( ++clockBankIndex == CLOCK_BANK_SIZE ) {\
			clockBankIndex = 0;\
			RTC_CHIP_SELECT = 1;\
		} else {\
			SSPBUF = clockBankIndex;\
		}\
	}

#endif
