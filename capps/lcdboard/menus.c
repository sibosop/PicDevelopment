#include <pic.h>
#include <sys.h>
#include "a2d.h"
#include "lcd.h"
#include "menus.h"
#include "lcdboard.h"
#include "timers.h"

#define FUNCTION_KNOB	0
#define CURSOR_KNOB		1
#define VALUE_KNOB		2

UInt8 lastVal;
UInt8 lastButt;
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



#define SLOP		4
#define FUNC_UP		20
#define FUNC_DOWN 	86	
#define CURSOR_UP 	42
#define CURSOR_DOWN	104
#define VAL_UP		63
#define VAL_DOWN	127
#define NO_BUTT		0

#define IS_BUTTON(x,v)	((v < ((x)+SLOP)) && (v > ((x)-SLOP)))
#define TEST_LAST_BUTT(x)	\
	if ( lastButt == x ) return; \
	else lastButt = x

void
checkMenus()
{
	UInt8 newVal;
	UInt8 tmp = 0;
	UInt8 i;

	newVal = getA2D(0) >> 1;
	tmp = (newVal > lastVal) ? newVal-lastVal : lastVal - newVal;
	if ( tmp > SLOP )
	{
		lastVal = newVal;
		setTimer(INPUT_TIMER,10);
		return;
	}

	if ( (tmp = IS_BUTTON(FUNC_UP,newVal)) || IS_BUTTON(FUNC_DOWN,newVal))
	{
		if ( tmp )
		{
			TEST_LAST_BUTT(FUNC_UP);
			if ( currentFunction == (getMenuSize()-1) ) 
				currentFunction = 0;
			else
				++currentFunction;
		}
		else
		{
			TEST_LAST_BUTT(FUNC_DOWN);
			if ( currentFunction == 0 )
				currentFunction = (getMenuSize() - 1);
			else
				--currentFunction;
		}

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
	else if ( (tmp = IS_BUTTON(CURSOR_UP,newVal)) || IS_BUTTON(CURSOR_DOWN,newVal))
	{
		if ( tmp )
		{
			TEST_LAST_BUTT(CURSOR_UP);
			++cursorPos;
			if ( cursorPos == 4 )
				cursorPos = 0;
		}
		else
		{
			TEST_LAST_BUTT(CURSOR_DOWN);
			--cursorPos;
			if ( cursorPos == 255 )
				cursorPos = 0;
		}
		placeCursor();
	}
	else if ( (tmp = IS_BUTTON(VAL_UP,newVal)) || IS_BUTTON(VAL_DOWN,newVal))
	{
		menuArgs.cursor = cursorPos;
		if ( tmp )
		{
			TEST_LAST_BUTT(VAL_UP);
			menuArgs.value = 1;
		}
		else
		{
			TEST_LAST_BUTT(VAL_DOWN);
			menuArgs.value = 0;
		}
		if ( menus[currentFunction].setValue ) menus[currentFunction].setValue();
	}
	else
	{
		lastButt = NO_BUTT;
	}
}
