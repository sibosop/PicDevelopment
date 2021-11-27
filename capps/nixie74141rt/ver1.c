
#include <pic.h>
#include "x8types.h"
#include "nixie74141.h"
#include "rtclock.h"
uint8_t	hours;
uint8_t	minutes;
uint16_t 	seconds;
void
displayHours() {
	nixieVals[NIXIE_HOUR_LSB] = hours % 10;
	nixieVals[NIXIE_HOUR_MSB] = hours / 10;
}

void 
displayMinutes() {
	nixieVals[NIXIE_MIN_LSB] = minutes % 10;
	nixieVals[NIXIE_MIN_MSB] = minutes / 10;
}

void 
displaySeconds() {
	duty1 = seconds * 6;
}

void
setHours() {
	hours = bcd2Bin(rtcGetHour());	
}



void
setMinutes() {
	minutes = bcd2Bin(rtcGetMin());
	
}

void
setSeconds() {
	seconds = bcd2Bin(rtcGetSecond());
}


void
versionInit(){
	hours = 10;
	minutes = 16;
	seconds = 0;
}


