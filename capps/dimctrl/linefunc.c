

#include "types.h"
#include "timers.h"
#include "linefunc.h"
#include "midi.h"
#include "menus.h"
#include "lcd.h"
#include "dimctrl.h"

UInt8 	noteBase;
UInt8	midiChannel;
bank3 LineVals lineVals[NUM_LINES];
UInt8 	lineFuncEnabled = 0;
UInt8	currentNote;
UInt8	currentVel;
UInt8  	newNote;

void doAuto(void) 
{
	lineFuncEnabled = 1;
	lcdEnableCursor(0);
}

void doCursorOn(void) 
{
	lcdEnableCursor(1);
}


void 
setManual(void)
{
	UInt8 val = menuArgs.value;

	lineFuncEnabled = 0;
	if ( val > 80 )
	{
		if (  lineVals[menuArgs.cursor].curVal != 127 )
			lineVals[menuArgs.cursor].curVal += 1;
	}
	else if ( val < 40 )
	{
		if (  lineVals[menuArgs.cursor].curVal != 0 )
			lineVals[menuArgs.cursor].curVal -= 1;
	}
	currentNote = 60 + menuArgs.cursor;
	currentVel  = lineVals[menuArgs.cursor].curVal;
	newNote = 1;
	setMenuValueAtCursor(lineVals[menuArgs.cursor].curVal);
	setTimer(INPUT_TIMER, 125);
}

UInt8
getManual(void)
{
	return lineVals[menuArgs.cursor].curVal;
}


void setMaxTime(void)
{
	lineFuncEnabled = 0;
	if ( menuArgs.value == 127 )
	{
		if ( lineVals[menuArgs.cursor].maxLineTime != 127 )
			lineVals[menuArgs.cursor].maxLineTime += 1;
	}
	else if ( menuArgs.value == 0 )
	{
		if ( lineVals[menuArgs.cursor].maxLineTime 
			> lineVals[menuArgs.cursor].minLineTime )
		{
			if ( lineVals[menuArgs.cursor].maxLineTime != 0 )
				lineVals[menuArgs.cursor].maxLineTime -= 1;
		}
	}
	setMenuValueAtCursor(lineVals[menuArgs.cursor].maxLineTime);
}

UInt8 getMaxTime(void)
{
	return lineVals[menuArgs.cursor].maxLineTime;
}

void setMinTime(void)
{
	lineFuncEnabled = 0;
	{
		if ( menuArgs.value == 127 )
		{
			if ( lineVals[menuArgs.cursor].maxLineTime 
				> lineVals[menuArgs.cursor].minLineTime )
			{
				if ( lineVals[menuArgs.cursor].minLineTime != 127 )
					lineVals[menuArgs.cursor].minLineTime += 1;
			}
		}
		else if ( menuArgs.value == 0 )
		{
			if ( lineVals[menuArgs.cursor].minLineTime != 0 )
				lineVals[menuArgs.cursor].minLineTime -= 1;
		}
	}
	setMenuValueAtCursor(lineVals[menuArgs.cursor].minLineTime);
}

UInt8 getMinTime(void)
{
	return lineVals[menuArgs.cursor].minLineTime;
}

void setMaxLimit(void)
{
	lineFuncEnabled = 0;
	if ( menuArgs.value == 127 )
	{
		if ( lineVals[menuArgs.cursor].maxBright != 127 )
			lineVals[menuArgs.cursor].maxBright += 1;
	}
	else if ( menuArgs.value == 0 )
	{
		if ( lineVals[menuArgs.cursor].maxBright 
				> lineVals[menuArgs.cursor].minBright )
		{
		if ( lineVals[menuArgs.cursor].maxBright != 0 )
			lineVals[menuArgs.cursor].maxBright -= 1;
		}
	}
	setMenuValueAtCursor(lineVals[menuArgs.cursor].maxBright);
}
UInt8 getMaxLimit(void)
{
	return lineVals[menuArgs.cursor].maxBright;
}

void setMinLimit(void)
{
	lineFuncEnabled = 0;
	if ( menuArgs.value == 127 )
	{
		if ( lineVals[menuArgs.cursor].maxBright 
				> lineVals[menuArgs.cursor].minBright )
		{
			if ( lineVals[menuArgs.cursor].minBright != 127 )
				lineVals[menuArgs.cursor].minBright += 1;
		}
	}
	else if ( menuArgs.value == 0 )
	{
		if ( lineVals[menuArgs.cursor].minBright != 0 )
			lineVals[menuArgs.cursor].minBright -= 1;
	}
	setMenuValueAtCursor(lineVals[menuArgs.cursor].minBright);
}

UInt8 getMinLimit(void)
{
	return lineVals[menuArgs.cursor].minBright;
}


LOOKUP_ENTRY menus[] =
{
	/*1234567890123456*/
	{"Auto Play       ",doAuto,MENU_NULL,MENU_NULL},
	{"Manual Play     ",doCursorOn,setManual,getManual},
	{"Set Max Times   ",doCursorOn,setMaxTime,getMaxTime},
	{"Set Min Times   ",doCursorOn,setMinTime,getMinTime},
	{"Set Max Limit   ",doCursorOn,setMaxLimit,getMaxLimit},
	{"Set Min Limit   ",doCursorOn,setMinLimit,getMinLimit}
};

#define NUM_MENUS (sizeof(menus)/sizeof(LOOKUP_ENTRY))
UInt8 getMenuSize() { return NUM_MENUS; }

#ifdef NO_PICC
#include <stdio.h>
#define DEBUG(a)	printf a
#include <stdlib.h>
UInt16 _random(UInt16 low,UInt16 high)
{
	UInt16 val;
	val = random() % (high - low);
	val += low;
	return val;
}
int linecnt = 0;
void putNote(UInt8 note, UInt8 vel, UInt8 chan)
{
	if ( chan ) 
		DEBUG(("putNote: %d %d %d\n", ++linecnt, note,vel));
	else
		DEBUG(("no note %d\n", ++linecnt, note,vel));
}
#else
#include "random.h"
#define _random	random
#define DEBUG(a)
#endif




void
setNewLine(UInt8 i)
{
	bank3 LineVals *lp = &lineVals[i];
	// determine the next  destination
	UInt8 newDest = _random(lp->minBright,lp->maxBright);

	DEBUG(("newdest = %d\n", newDest));

	lp->addFlag = newDest > lp->dest;
	lp->destCount = lp->addFlag ? newDest-lp->dest : lp->dest-newDest;
	lp->grainCount = _random(lp->minLineTime,lp->maxLineTime);
	DEBUG(("newcount = %d\n", lp->grainCount));
#ifdef NO_PICC
	linecnt = 0;
#endif

	lp->numGrains = lp->grainCount;
	lp->div = lp->destCount / lp->grainCount;	
	lp->mod = lp->destCount % lp->grainCount;
	lp->modSum = 0;
	lp->curVal = lp->dest;
	lp->dest = newDest;
}

UInt8
checkLine(UInt8 i)
{
	bank3 LineVals *lp = &lineVals[i];
	UInt8 curVal = lp->curVal;
	if ( !lp->grainCount )
		return 1;

	if ( lp->addFlag )
		lp->curVal += lp->div;
	else 
		lp->curVal -= lp->div;

	lp->modSum += lp->mod;
	if ( lp->modSum >= lp->numGrains )
	{
			if ( lp->addFlag )
				lp->curVal++;
			else
				lp->curVal--;
			lp->modSum -= lp->numGrains;
	}
	lp->grainCount--;	
	if ( curVal != lp->curVal )
		putNote(noteBase+i,lp->curVal,midiChannel);
#ifdef NO_PICC
	else
		putNote(noteBase+i,lp->curVal,0);
#endif 
	return 0;
}

void
checkLines(void)
{
	UInt8 i;
	UInt8 tmp;
	noteBase = DEFAULT_NOTE_BASE;
	midiChannel = DEFAULT_MIDI_CHANNEL;
	if ( !lineFuncEnabled ) return;

	for (i = 0; i < NUM_LINES; ++i )
	{
		if ( checkLine(i) )
			setNewLine(i);
	}
	for ( i = 0; i < 4 ; ++i )
	{
		setMenuCursorPos(3-i);
		menuArgs.cursor = 3-i;
		tmp = lineVals[3-i].curVal;
		setMenuValueAtCursor(tmp);
	}
}

void
initLines(void)
{
	UInt8 i;
#ifdef NO_PICC
	srandom(time(0));
#endif
	lineFuncEnabled = 0;

	
	for ( i = 0; i < NUM_LINES; ++i )
	{
		lineVals[i].minLineTime = DEFAULT_MIN_LINE_TIME;
		lineVals[i].maxLineTime = DEFAULT_MAX_LINE_TIME;
		lineVals[i].maxBright = DEFAULT_MAX_BRIGHT;
		lineVals[i].minBright = DEFAULT_MIN_BRIGHT;
		setNewLine(i);
	}
}
