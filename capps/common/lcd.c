#include <sys.h>
#include <pic.h>
#include "types.h"
#include "lcd.h"
//#include "lcdtext.h"

#define LCD_RS	RA2
#define LCD_RW	RA4
#define LCD_E	RA5

#define LCD_PULSE_E \
	LCD_E = 1;	\
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

UInt8
lcdBusyWait()
{
	UInt8 retVal;
	TRISB = 0xff;	// send flag to read
	TRISC |= 0x30;
	
	do {
		LCD_RS = 0;
		LCD_RW = 1;
		LCD_PULSE_E;
	} while ( RC5 != 0 );
	retVal = RC4;
	retVal <<= 6;
	retVal |= PORTB & 0x3F; 
	RC4 = 0;
	RC5 = 0;
	PORTB = 0;
	TRISB = 0;
	TRISC &= 0xCF;
	return retVal;
}

void
lcdSendCommand(UInt8 val)
{
	lcdBusyWait();
	LCD_RS = 0;
	LCD_RW = 0;
	PORTB = val & 0x3f;
	RC4 = (val & 0x40) ? 1 : 0;
	RC5 = (val & 0x80) ? 1 : 0;
	LCD_PULSE_E;
}

void
lcdSendData(UInt8 val)
{
	lcdBusyWait();
	LCD_RS = 1;
	LCD_RW = 0;
	PORTB = val & 0x3f;
	RC4 = (val & 0x40) ? 1 : 0;
	RC5 = (val & 0x80) ? 1 : 0;
	LCD_PULSE_E;
}

void
lcdInitDisplay1()
{
	LCD_E = 0;
	PORTB = 0;
	RC4 = 0;
	RC5 = 0;
	// Function set
	// 2 - line mode
	LCD_RS = 0;
	LCD_RW = 0;
	PORTB = 0x38;
	RC4 = 0;
	RC5 = 0;
	LCD_PULSE_E;
}

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
lcdInitDisplay2()
{
	// at this point the
	// busy flag should be working
	lcdSendCommand(0x38);
	

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

