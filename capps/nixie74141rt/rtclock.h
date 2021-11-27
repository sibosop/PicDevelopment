#ifndef rt_clock_h
#define rt_clock_h
#include <pic.h>
//#include <sys.h>
#include <stdint.h>
#include "x8types.h"
#define	RTC_CHIP_SELECT	RC1
#define RTC_MSO_DIR	TRISC5
#define RTC_12_HOUR_BIT	0x40
#define RTC_AM_PM_BIT	0x20
#define CLOCK_BANK_SIZE	0x0F
void rtcInit();
void rtcWrite(uint8_t data, uint8_t addr);
uint8_t rtcRead(uint8_t addr);
void rtcInterrupt();
void rtcStart();
uint8_t	rtcVal(uint8_t i);
extern uint8_t	clockBankIndex;
extern uint16_t	secCount;
extern bank3 uint8_t clockBank[CLOCK_BANK_SIZE];
extern uint8_t clockBankIndex;
extern Boolean	addrFlag;	
extern Boolean stopFlag;
extern Boolean inProcess;

uint8_t rtcCheck();
uint8_t	rtcGetMin();
uint8_t	rtcGetHour();
uint8_t	rtcGetWeekday();
uint8_t	rtcGetMon();
uint8_t	rtcGetDate();
uint8_t	rtcGetCentury();
uint8_t	rtcGetYear();
uint8_t	rtcIsPm();
void	rtcStop();
void 	rtcStart();
void	rtcRestart();
void	rtcBumpMin();
void	rtcBumpHour();
uint8_t	rtcGetSecond();
void	rtcBumpYear();
void	rtcBumpDate();
void	rtcBumpMon();
void	rtcBumpWeekday();
void	rtcClearSec();
uint8_t	bcd2Bin(uint8_t);
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
		if ( ++clockBankIndex >= CLOCK_BANK_SIZE ) {\
			clockBankIndex = 0;\
			RTC_CHIP_SELECT = 1;\
			inProcess = 0; \
		} else {\
			SSPBUF = clockBankIndex;\
		}\
	}

#endif
