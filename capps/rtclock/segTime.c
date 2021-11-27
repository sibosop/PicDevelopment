static UInt8  lastMin = 0;
static UInt8  lastSec = 0;
static UInt8  displayState = 0;
static UInt8  displayCnt = 9;


UInt16
displayHour() {
	UInt16 displayVar = rtcGetHour() << 8;
	segDotHi = rtcIsPm();
	displayVar |= rtcGetMin();
	return displayVar;
} 

UInt16
displayDate() {
	UInt16 displayVar = rtcGetMon() << 8;
	displayVar |= rtcGetDate();
	segDotHi = 0;
	return displayVar;
}

UInt16
displayYear() {
	UInt16 displayVar = rtcGetCentury() << 8;
	displayVar |= rtcGetYear();
	segDotHi = 0;
	return displayVar;
}

void 
displayTime() {
	UInt16 displayVar;
	switch(runMode) {	
		case DisplayNormal:
			if ( ++displayCnt == 10 ) {
				displayCnt = 0;
				if ( ++displayState == 3 ) {
					displayState = 0;
				}	
			}
			switch(displayState) {
				default:
				case 0:
					displayVar = displayHour();
					break;
				case 1:
					displayVar = displayDate();
					break;
				case 2:
					displayVar = displayYear();
					break;
			}					
		break;
			
		case DisplayTime:
			displayVar = displayHour();	
		break;
		
		case DisplayDate:
			displayVar = displayDate();
		break;

		case DisplayYear:
			displayVar = displayYear();	
		break;
	} 
	segDisplay(displayVar);
}