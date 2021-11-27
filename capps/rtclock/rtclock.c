#include "rtclock.h"


bank3 UInt8 clockBank[CLOCK_BANK_SIZE];
UInt8 clockBankIndex;
Boolean	addrFlag;	
Boolean stopFlag;



void rtcInit() {
	UInt8	tmp;
	RTC_CHIP_SELECT	= 1;
	TRISA0 = 0;	// logic analyzer trigger
	TRISC2 = 0; // chip select ouput
	TRISC3 = 0; // clock output
	TRISC4 = 1; // spl input
	TRISC5 = 0; // spl output

	SSPCON = 1;  // rate = clock/16
	SSPEN = 1; // enable serial ports
	SSPIE = 1;	// enable serial port interrupt
	GIE = 1;	// enable general interrupts
	PIE1 = 1;	// enable peripheral interrupt
	STAT_SMP = 0; // sample at data edge
	CKP = 0; // clock on rising edge
	STAT_CKE = 0;
	clockBankIndex = 0;
	addrFlag = 1;
	rtcWrite(0xe,0x68);
	tmp = rtcRead(0x2);
	rtcWrite(0x2,RTC_12_HOUR_BIT|tmp);
	//rtcWrite(1,0x59);
	TRISB0 = 1;
	secCount = 1024;
	stopFlag = 0;
	INTE = 1; 	// enable the RB0 interrupt
	TMR0 = 0x10;
}
UInt8 
bcd2Bin(UInt8 v) {
	return (((v >> 4)*10) + (v & 0xf));
}

UInt8
bin2Bcd(UInt8 v) {
	return ((v / 10) << 4) + (v % 10);
}

UInt8	
rtcVal(UInt8 i) {
	return clockBank[i];
}

void rtcStart() {
	if ( stopFlag )
		return;
	addrFlag = 1;
	RTC_CHIP_SELECT = 0; 
	RTC_MSO_DIR  = 0; 
	SSPBUF = clockBankIndex;
}


void rtcStop() {
//	while(clockBankIndex != 0);
	stopFlag = 1;
}

void rtcRestart() {
	stopFlag = 0;
	
}

UInt8 
rtcRead(UInt8 addr) {
	UInt8 rval;
	RTC_MSO_DIR = 0;
	RTC_CHIP_SELECT = 0;
	// Write the addr
	SSPBUF = addr;	
	while ( !STAT_BF ) {
	}

	// Initiate a read by
	// writing a 0 to the
	// disabled MS0 pin.
	// This will clock in the
	// data 
	RTC_MSO_DIR = 1;
	SSPBUF = 0;
	while( !STAT_BF ) {
	}		
	rval = SSPBUF;
	RTC_CHIP_SELECT = 1;
	return rval;
}

void
rtcWrite(UInt8 addr, UInt8 data) {
	RTC_MSO_DIR = 0;
	RTC_CHIP_SELECT = 0;
	SSPBUF = addr | 0x80;
	while( !STAT_BF ) {
	}
	addr= SSPBUF;
	SSPBUF = data;
	while( !STAT_BF ) {
	}
	addr = SSPBUF;
	RTC_CHIP_SELECT = 1;
}

UInt8
rtcGetMin() {
	return clockBank[RTC_MIN_REG];
}

UInt8
rtcGetHour() {
	return clockBank[RTC_HOUR_REG] & 0x1f;
}

UInt8
rtcGetSecond() {
	return clockBank[RTC_SEC_REG];
}

UInt8
rtcIsPm() {
	return (clockBank[RTC_HOUR_REG] & RTC_AM_PM_BIT) != 0;
}

UInt8
rtcGetWeekday() {
	return clockBank[RTC_WEEKDAY_REG];
}

UInt8
rtcGetMon() {
	return clockBank[RTC_MON_REG] & 0x1f;
}

UInt8
rtcGetDate() {
	return clockBank[RTC_DATE_REG] & 0x3f;
}

UInt8
rtcGetYear() {
	return clockBank[RTC_YEAR_REG];
}

UInt8
rtcGetCentury() {
	return 0x20;
}

const UInt8 const dateTab[] = {
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
void
rtcBumpDate() {
	UInt8	y = bcd2Bin(rtcGetYear());
	UInt8	m = bcd2Bin(rtcGetMon());
	UInt8	d = bcd2Bin(rtcGetDate());
	
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

void
rtcBumpMon() {
	UInt8	m = bcd2Bin(rtcGetMon());

	if ( ++m > 12 )
		m = 1;
	m = bin2Bcd(m) | 0x80;

	rtcWrite(RTC_MON_REG,m);
	clockBank[RTC_MON_REG]=m;
}

void
rtcBumpWeekday() {
	UInt8 m = rtcGetWeekday();
	if ( ++m > 7 ) 
		m = 1;
	rtcWrite(RTC_WEEKDAY_REG,m);
	clockBank[RTC_WEEKDAY_REG]=m;
}

void
rtcBumpYear() {
	UInt8 y = bcd2Bin(rtcGetYear());
	if ( ++y == 100 )
		y = 0;
	y = bin2Bcd(y);
	rtcWrite(RTC_YEAR_REG,y);
	clockBank[RTC_YEAR_REG] = y;
}
void
rtcBumpMin() {
	UInt8 lo,hi;		
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
	UInt8 hour = bcd2Bin(rtcGetHour());
	UInt8 set = RTC_12_HOUR_BIT;
	
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
