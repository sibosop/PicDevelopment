
#ifndef LINEFUNC_H
#define LINEFUNC_H

#define GRAIN_TIME	100		// milliseconds
#define GRAIN_TIMER	0

#ifdef NO_PICC
#define NUM_LINES	1
#define DEFAULT_MIDI_CHANNEL	0
#define DEFAULT_NOTE_BASE		60
#define DEFAULT_MAX_LINE_TIME	100
#define DEFAULT_MIN_LINE_TIME	10
#define DEFAULT_MIN_BRIGHT		0
#define DEFAULT_MAX_BRIGHT		127
#else
#define NUM_LINES	4
#define DEFAULT_MIDI_CHANNEL	0
#define DEFAULT_NOTE_BASE		60
#define DEFAULT_MAX_LINE_TIME	100
#define DEFAULT_MIN_LINE_TIME	10
#define DEFAULT_MIN_BRIGHT		20
#define DEFAULT_MAX_BRIGHT		127
#endif

typedef struct 
{
	UInt8	addFlag;
	UInt8	div;
	UInt8	mod;
	UInt16	modSum;
	UInt8	dest;
	UInt8	curVal;
	UInt8	destCount;
	UInt16	grainCount;
	UInt16 	numGrains;
	UInt16	maxLineTime;
	UInt16	minLineTime;
	UInt16	maxBright;
	UInt16	minBright;
} LineVals;

extern void checkLines(void);
extern void setNewLine(UInt8 i);
extern UInt8 checkLine(UInt8 i);
extern void initLines(void);
extern void	checkEeprom(void);

#endif
