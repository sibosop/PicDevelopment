#include "rtclock.h"


bank3 uint8_t clockBank[CLOCK_BANK_SIZE];
uint8_t clockBankIndex;
Boolean	addrFlag;	
uint8_t	rtcStopCount;
typedef enum rtcState {
	InitStart
	,ResetAddr
	,AddrWait
	,ReadyWait
	,ReadData
	,Idle
} RtcState;
RtcState rtcState;
void rtcInit() {
//	uint8_t	tmp;
	RTC_CHIP_SELECT	= 1;
	TRISA0 = 0;	// logic analyzer trigger
	TRISC1 = 0; // chip select ouput
	TRISC3 = 0; // clock output
	TRISC4 = 1; // spl input
	TRISC5 = 0; // spl output

	SSPCON = 2;  // rate = clock/64
	SSPEN = 1; // enable serial ports
//	SSPIE = 1;	// enable serial port interrupt
	GIE = 1;	// enable general interrupts
	PIE1 = 1;	// enable peripheral interrupt
	SMP = 0; // sample at data edge
	CKP = 0; // clock on rising edge
	CKE = 0;
	clockBankIndex = 0;
	addrFlag = 1;
	rtcWrite(0xe,0x48);
//	tmp = rtcRead(0x2);
//	rtcWrite(0x2,RTC_12_HOUR_BIT|tmp);
	//rtcWrite(1,0x59);
	//secCount = 1024;
	rtcWrite(0xA,0x55);	// sanity check
	rtcStopCount = 0;
	rtcState = Idle;
}
uint8_t 
bcd2Bin(uint8_t v) {
	return ((uint8_t)(((v >> 4)*10) + (v & 0xf)));
}

uint8_t
bin2Bcd(uint8_t v) {
	return ((uint8_t)((v / 10) << 4) + (v % 10));
}
#if 0
uint8_t	
rtcVal(uint8_t i) {
	return clockBank[i];
}
#endif



void
rtcStart() {
	if ( !rtcStopCount || !(--rtcStopCount) )
		rtcState = InitStart;
}
	
uint8_t rtcCheck() {
	uint8_t rval = 0;
	di();
	switch ( rtcState ) {
		default:
			rtcState = InitStart;
			break;
		case Idle:
			break;
		case InitStart:
			clockBankIndex = 0;
			RTC_CHIP_SELECT = 0;
			RTC_MSO_DIR = 0;
			rtcState = ResetAddr;
			break;

		case ResetAddr:
			SSPBUF = clockBankIndex;
			
			rtcState = AddrWait;
			break;

		case AddrWait:
			if (BF) {
				RTC_MSO_DIR = 1;
				SSPBUF = 0;
				RTC_MSO_DIR = 0;
				rtcState  = ReadyWait;
			}
			break;

		case ReadyWait:
			if ( BF ) 
				rtcState = ReadData;	
			break;
		case ReadData:
			clockBank[clockBankIndex] = SSPBUF;
			if ( clockBankIndex >= CLOCK_BANK_SIZE ) {
				rtcState = Idle;
				//rtcTimer = 0x100;		
				RTC_CHIP_SELECT = 1;
				if ( clockBank[0xA] == 0x55 )
					rval = 1;
				else
					rval = 0;
			} else {
				++clockBankIndex;
				//RTC_MSO_DIR = 1;
				SSPBUF = 0x00;
				//RTC_MSO_DIR = 0;
				rtcState = ReadyWait;
			}
			break;
	}
	ei();
	return rval;
}
		

void rtcStop() {
	rtcState = Idle;
	rtcStopCount = 4;
}
#if 0
void rtcRestart() {
	rtcState = InitStart;
}
#endif

uint8_t 
rtcRead(uint8_t addr) {
	uint8_t rval;
	RTC_MSO_DIR = 0;
	RTC_CHIP_SELECT = 0;
	// Write the addr
	SSPBUF = addr;	
	while ( !BF ) {
	}

	// Initiate a read by
	// writing a 0 to the
	// disabled MS0 pin.
	// This will clock in the
	// data 
	RTC_MSO_DIR = 1;
	SSPBUF = 0;
	while( !BF ) {
	}		
	rval = SSPBUF;
	RTC_CHIP_SELECT = 1;
	return rval;
}

void
rtcWrite(uint8_t addr, uint8_t data) {
	RTC_MSO_DIR = 0;
	RTC_CHIP_SELECT = 0;
	SSPBUF = addr | 0x80;
	while( !BF ) {
	}
	addr= SSPBUF;
	SSPBUF = data;
	while( !BF ) {
	}
	addr = SSPBUF;
	RTC_CHIP_SELECT = 1;
}

uint8_t
rtcGetMin() {
	return clockBank[RTC_MIN_REG];
}

uint8_t
rtcGetHour() {
	return clockBank[RTC_HOUR_REG] & 0x1f;
}

uint8_t
rtcGetSecond() {
	return clockBank[RTC_SEC_REG];
}

uint8_t
rtcIsPm() {
	return (clockBank[RTC_HOUR_REG] & RTC_AM_PM_BIT) != 0;
}
#if 0
uint8_t
rtcGetWeekday() {
	return clockBank[RTC_WEEKDAY_REG];
}

uint8_t
rtcGetMon() {
	return clockBank[RTC_MON_REG] & 0x1f;
}

uint8_t
rtcGetDate() {
	return clockBank[RTC_DATE_REG] & 0x3f;
}

uint8_t
rtcGetYear() {
	return clockBank[RTC_YEAR_REG];
}

uint8_t
rtcGetCentury() {
	return 0x20;
}
#endif

const uint8_t dateTab[] = {
	0
	,31  //Jan
	,0
	,31	 //Mar
	,30  //Apr
	,31  //May
	,30  //June
	,31  //July
	,31  //Aug
	,30  //Sep
	,31  //Oct
	,30  //Nov
	,31  //Dec
};

#if 0
void
rtcBumpDate() {
	uint8_t	y = bcd2Bin(rtcGetYear());
	uint8_t	m = bcd2Bin(rtcGetMon());
	uint8_t	d = bcd2Bin(rtcGetDate());
	
	++d;
	
	if ( (m==2 && y % 4 && d == 29)
		|| (m==2 && !(y % 4) && d == 30 ) 
		|| (m != 2 && d > dateTab[m]) ) {
			d = 1;
	}
	d = bin2Bcd(d);
	rtcWrite(RTC_DATE_REG,d);
	clockBank[RTC_DATE_REG] = d;
}
#endif

void
rtcClearSec() {
	rtcWrite(RTC_SEC_REG,0);
	clockBank[RTC_SEC_REG]=0;
}
#if 0
void
rtcBumpMon() {
	uint8_t	m = bcd2Bin(rtcGetMon());

	if ( ++m > 12 )
		m = 1;
	m = bin2Bcd(m) | 0x80;

	rtcWrite(RTC_MON_REG,m);
	clockBank[RTC_MON_REG]=m;
}

void
rtcBumpWeekday() {
	uint8_t m = rtcGetWeekday();
	if ( ++m > 7 ) 
		m = 1;
	rtcWrite(RTC_WEEKDAY_REG,m);
	clockBank[RTC_WEEKDAY_REG]=m;
}

void
rtcBumpYear() {
	uint8_t y = bcd2Bin(rtcGetYear());
	if ( ++y == 100 )
		y = 0;
	y = bin2Bcd(y);
	rtcWrite(RTC_YEAR_REG,y);
	clockBank[RTC_YEAR_REG] = y;
}
#endif
void
rtcBumpMin() {
	uint8_t lo,hi;		
	hi = rtcRead(RTC_MIN_REG);
	lo = hi & 0xf;
	hi &= 0xf0;
	if ( ++lo > 9 ) {
		lo = 0;
		hi += 0x10;
		if ( hi > 0x50 ) 
			hi = 0;
	}
	rtcWrite(RTC_MIN_REG,hi | lo);
	clockBank[RTC_MIN_REG] = hi | lo;
}

void
rtcBumpHour() {
	uint8_t hour = bcd2Bin(rtcGetHour());
	uint8_t set = RTC_12_HOUR_BIT;
	
	++hour;
	if ( hour == 12 ) {
		if ( rtcIsPm() ) {
			clockBank[RTC_HOUR_REG] &= ~RTC_AM_PM_BIT; 
		} else {
			clockBank[RTC_HOUR_REG] |= RTC_AM_PM_BIT;
		}
	}
	if ( hour > 12 ) {
		hour = 1;
	}
	set |= clockBank[RTC_HOUR_REG] & RTC_AM_PM_BIT;
	set |= bin2Bcd(hour);
	rtcWrite(RTC_HOUR_REG,set);
	clockBank[RTC_HOUR_REG] = set & 0x3f;
}	
