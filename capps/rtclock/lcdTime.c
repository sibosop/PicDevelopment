#include <pic.h>
#include "types.h"
#include "rtclock.h"
#include "lcd.h"
#include "rtcbutton.h"
#include "worddisp.h"

const char* const month[] = {
	"Jan"
	,"Feb"
	,"Mar"
	,"Apr"
	,"Jun"
	,"Jul"
	,"Aug"
	,"Sep"
	,"Oct"
	,"Nov"
	,"Dec"
};


const char* const day[] = {
	"___"
	,"Mon"
	,"Tue"
	,"Wed"
	,"Thu"
	,"Fri"
	,"Sat"
	,"Sun"
};

const char* const AM_PM[] = {
	"AM"
	,"PM"
}; 
SetMode lastMode = LastMode;

void displayTime()
{
	//lcdSendCommand(0x80);
	lcdSetCursor(0,0);
	lcdSendMessage(day[rtcGetWeekday()]);
	lcdSendData(' ');
	lcdSend2DigitNum(bcd2Bin(rtcGetMon()));
	lcdSendData('/');
	lcdSend2DigitNum(bcd2Bin(rtcGetDate()));
	lcdSendData(' ');
	lcdSend2DigitNum(bcd2Bin(rtcGetHour()));
	lcdSendData(':');
	lcdSend2DigitNum(bcd2Bin(rtcGetMin()));	

	if ( rtcIsPm() )
		lcdSendData('P');
	else
		lcdSendData('A');

	
	if ( lastMode != setMode ) {
		lastMode = setMode;
		lcdSetCursor(0,1);
		lcdSendMessage("                ");
	}
	lcdSetCursor(0,1);
	switch ( setMode ) {
		default:
		case Running:
			putWords();
		break;

		case DisplayTime:
			lcdSendMessage("Set Time");
			break;

		case DisplayDate:
			lcdSendMessage("Set Date");
			break;

		case DisplayYear:
			lcdSendMessage("Set Year: ");
			lcdSend2DigitNum(bcd2Bin(rtcGetYear()));
			break;

		case DisplayWeekday:
			lcdSendMessage("Set WeekDay");
			break;

		case DisplayAlarm1:
			lcdSendMessage("Set Alarm 1 (not imp)");
			break;

		case DisplayAlarm2:
			lcdSendMessage("Set Alarm 2 (not imp)");
			break;
	}
}



