#include <sys.h>
#include <pic.h>
#include "types.h"
#include "lcd.h"
//#include "lcdtext.h"

#define LCD_RS	RC0
#define LCD_RS_TRIS	TRISC0
#define LCD_RW	RB5
#define LCD_RW_TRIS	TRISB5
#define LCD_E	RC6
#define LCD_E_TRIS	TRISC6

static UInt8	tc;
#define LCD_PULSE_E \
	LCD_E = 1;	\
	for ( tc = 5; tc ; --tc ) \
		asm("nop"); \
	LCD_E = 0


bank2 UInt8	DCBMask;	// Display Cursor Blink enable 

#define DCB_COMMAND	8
#define BLINK_BIT	1
#define CURSOR_BIT	2
#define DISPLAY_BIT	4

#define BLINK_ON(x)		((x) |= BLINK_BIT)
#define BLINK_OFF(x)	((x) &= ~BLINK_BIT)
#define CURSOR_ON(x)	((x) |= CURSOR_BIT)
#define CURSOR_OFF(x)	((x) &= ~CURSOR_BIT)
#define DISPLAY_ON(x)	((x) |= DISPLAY_BIT)
#define DISPLAY_OFF(x)	((x) &= ~DISPLAY_BIT)


void LCD_READ_PORT(UInt8 x) {
	
}

static UInt8	wt;
static UInt8	wv;
#define LCD_WRITE_PORT() \
	wt = PORTB;\
	wt &= ~0x1E;\
	wt |= (wv & 0xF0) >> 3;\
	PORTB = wt; \
	LCD_PULSE_E;\
	wt &= ~0x1E; \
	wt |= (wv & 0x0F) << 1;\
	PORTB = wt; \
	LCD_PULSE_E
	
UInt8 rv;

#define lcdBusyWait() \
	TRISB |= 0x1E;\
	do {\
		LCD_RS = 0;\
		LCD_RW = 1;\
		LCD_PULSE_E;\
		rv = (PORTB & 0x1E) >> 1;\
		rv <<= 4; \
		LCD_PULSE_E;\
		rv |= (PORTB & 0x1E) >> 1;\
	} while ( rv & 0x80 );\
	PORTB &= ~0x1E;\
	TRISB &= ~0x1E;\

void
lcdSendCommand(UInt8 val)
{
	
	lcdBusyWait();
	LCD_RS = 0;
	LCD_RW = 0;
	wv = val;
	LCD_WRITE_PORT();
}

void
lcdSendData(UInt8 val)
{
	lcdBusyWait();
	LCD_RS = 1;
	LCD_RW = 0;
	wv = val;
	LCD_WRITE_PORT();
}

UInt8	lcdTimer;
UInt8	lcdState = 0;
void
lcdInitDisplay2()
{
	// at this point the
	// busy flag should be working
	// 4 bit mode display on 5-11
	lcdSendCommand(0x28);
	
	

	DCBMask = DCB_COMMAND;
	DISPLAY_ON(DCBMask);
	BLINK_OFF(DCBMask);
	CURSOR_OFF(DCBMask);
	lcdSendCommand(DCBMask);

	// clear display
	lcdSendCommand(0x01);

	// increment mode no shift
	lcdSendCommand(0x6);

	// set address
	lcdSetCursor(0,0);
}
#if 0
UInt8
checkLcd()
{
	switch(lcdState) {
		// initialize ports
		// wait 30 ms
		case 0:
			TRISA5 = 0;
			LCD_RS_TRIS = 0;
			LCD_RW_TRIS = 0;
			LCD_E_TRIS = 0;
			PORTB &= ~0x1E;
			lcdTimer = 30;
			break;

		// wait states
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
		case 11:
		case 13:
		case 15:
		case 17:
		case 19:
			if ( --lcdTimer ) 
				return 0;
			break;
		
		// write 20 hex wait 10 ms
		case 2:
		case 4:
			LCD_RS = 0;
			LCD_RW = 0;
			lcdTimer = 10;
			LCD_WRITE_PORT(0x20);
			break;

		// display on
		// two line mode
		case 6:
			lcdTimer = 1;
			LCD_WRITE_PORT(0xC0);
			break;
		// intermediate 0 writes for next three
		// setup parameters
		case 8:
		case 12:
		case 16:
			lcdTimer = 1;
			LCD_WRITE_PORT(0);
			break;
		// display on
		// cursor 0ff
		// blink off
		case 10:
			lcdTimer = 1;
			LCD_WRITE_PORT(0xC0);
			break;

		// display clear
		case 14:
			lcdTimer = 1;
			LCD_WRITE_PORT(0x1);
			break;

		// increment mode
		// shift off
		case 18:
			lcdTimer = 1;
			LCD_WRITE_PORT(0x6);
			break;

		case 20:
			lcdInitDisplay2();
			break;

		default:
			return 1;
	}		
	++lcdState;
	return 0;
}	
#else
bank2 UInt8 val;
UInt8
checkLcd()
{
	if ( !lcdState ) {
		TRISA5 = 0;
		LCD_RS_TRIS = 0;
		LCD_RW_TRIS = 0;
		LCD_E_TRIS = 0;
		PORTB &= ~0x1E;
		lcdTimer = 30;
		goto BumpState;
	}

	if ( lcdState % 2 ) {
		if ( --lcdTimer ) 
			return 0;
		goto BumpState;
	}

	if ( lcdState == 2 || lcdState == 4 ) {
		LCD_RS = 0;
		LCD_RW = 0;
		lcdTimer = 10;
		wv = 0x20;
		goto LcdWritePort;
	}
	if ( lcdState == 6 ) {
		lcdTimer = 1;
		wv = 0xc0;
		goto LcdWritePort;
	} 
	if ( lcdState == 8 
				|| lcdState == 12 
				|| lcdState == 16 ) {
		lcdTimer = 1;
		wv = 0;
		goto LcdWritePort;
	}
	if ( lcdState == 10 ) {
		lcdTimer = 1;
		wv = 0xc0;
		goto LcdWritePort;
	}
	if ( lcdState == 14 ) {		
		lcdTimer = 1;
		wv = 1;
		goto LcdWritePort;
	}
	if ( lcdState == 18 ){
		lcdTimer = 1;
		wv = 6;
		goto LcdWritePort;
	} 
	if ( lcdState == 20 ) {	
		lcdInitDisplay2();
		lcdTimer = 1;
		goto BumpState;
	} 
	return 1;
LcdWritePort:
	LCD_WRITE_PORT();
BumpState:	
	++lcdState;
	return 0;
}	
#endif

void
lcdSetCursor(UInt8 x, UInt8 y)
{
	UInt8 posCmd = 0x80;
	if ( y )
		posCmd += 0x40;
	posCmd += x & 0x3f;
	lcdSendCommand(posCmd);
}



void
lcdSendMessage(const char *msg)
{
	for(;*msg;++msg)
	{
		lcdSendData(*msg);
	}

}

void
lcdClearDisplay()
{
	lcdSendCommand(1);
}

void
lcdEnableShift(UInt8 dir)
{
	lcdSendCommand(dir ? 7 : 5);
}

void
lcdDisableShift()
{
	lcdSendCommand(6);
}

void
lcdSetCursorDir(UInt8 dir)
{
	lcdSendCommand(dir ? 6 : 4);
}


void
lcdSendDecimalNum(UInt8 num)
{
	if ( num >= 100 )
		lcdSendData('0' + (num/100));
	else
		lcdSendData(' ');
	if ( num >= 10 )
		lcdSendData('0' + ((num % 100) / 10 ));
	else
		lcdSendData(' ');
	lcdSendData('0' + (num % 10)); 
}

void
lcdSend2DigitNum(UInt8 num) {
	lcdSendData('0' + (num / 10));
	lcdSendData('0' + (num % 10));	
}

void
lcdEnableDisplay(UInt8 on)
{
	if ( on )
		DISPLAY_ON(DCBMask);
	else
		DISPLAY_OFF(DCBMask);
	lcdSendCommand(DCBMask);
}

void
lcdEnableCursor(UInt8 on)
{
	if ( on )
		CURSOR_ON(DCBMask); 
	else
		CURSOR_OFF(DCBMask);
	lcdSendCommand(DCBMask);
}

void
lcdEnableBlink(UInt8 on)
{
	if ( on )
		BLINK_ON(DCBMask);
	else
		BLINK_OFF(DCBMask);
	lcdSendCommand(DCBMask);
}

void
lcdMoveDisplay(UInt8 dir)
{
	lcdSendCommand(dir ? 0x1C : 0x18);
}

