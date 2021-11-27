#include <pic.h>
#include <sys.h>
#include "a2d.h"
#include "lcd.h"
#include "menus.h"

#define REVERSED
#define FUNCTION_KNOB	0
#define CURSOR_KNOB		1
#define VALUE_KNOB		2

UInt8 lastVal[3];
MenuArgs menuArgs;



UInt8 currentFunction = 0;
UInt8 cursorPos = 0;
UInt8 cursorOn = 0;
void placeCursor()
{
	lcdSetCursor(cursorPos * 4, 1 );
	lcdEnableCursor(1);
}


void setFunction(UInt8 newVal)
{
}

void setCursor(UInt8 newVal)
{
	UInt8 np = newVal / 20;
	np = np % 4;
	if ( np != cursorPos )
	{
		cursorPos = np;
		placeCursor();
	}
}

void setCursorValue(UInt8 newVal)
{
	lcdSendDecimalNum(newVal);
	if (cursorOn)
		placeCursor();
}


void setMenuValueAtCursor(UInt8 val)
{
	lcdSendDecimalNum(val);
	placeCursor();
}

void setMenuCursorPos(UInt8 pos)
{
	if ( pos != cursorPos )
	{
		cursorPos = pos;
		placeCursor();
	}
}



void
checkMenus()
{
	UInt8 knob;
	UInt8 newVal;
	UInt8 	s;
	UInt8 	f;
	UInt8 	i;
	UInt8	tmp;
	for ( knob = 0; knob < 3; ++knob )
	{
#ifdef REVERSED
		newVal = 255 - getA2D(knob);
#else
		newVal = getA2D(knob);
#endif
		newVal >>= 1;	// ignore bottom bit
		
		if ( newVal != lastVal[knob] )
		{
			lastVal[knob] = newVal;
			switch ( knob )
			{
				case FUNCTION_KNOB:
					s = getMenuSize();
					f = (newVal / 20); 
					f = f % s;
					if ( f != currentFunction )
					{
						currentFunction = f;
						lcdSetCursor(0,0);
						lcdSendMessage(menus[currentFunction].functionName);
						if ( menus[currentFunction].init )
							menus[currentFunction].init();
						if ( menus[currentFunction].getValue )
						{
							for ( i = 0; i < 4 ; ++i )
							{
								setMenuCursorPos(3-i);
								menuArgs.cursor = 3-i;
								tmp = menus[currentFunction].getValue();
								setMenuValueAtCursor(tmp);
							}
						}
						if ( cursorOn ) placeCursor();
					}
				break;

				case CURSOR_KNOB:
					setCursor(newVal);
				break;

				case VALUE_KNOB:
					menuArgs.cursor = cursorPos;
					menuArgs.value  = newVal;
					if ( menus[currentFunction].setValue ) menus[currentFunction].setValue();
				break;
			}
		}
	}
}
