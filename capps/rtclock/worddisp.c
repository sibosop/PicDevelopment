#include <pic.h>
#include <sys.h>
#include "pseudorand.h"
#include "worddisp.h"
#include "lcd.h"



bank2 UInt8	numWords;
extern const char * const words[];

void
initWordDisp()
{
	for ( numWords=0; words[numWords] != NULL ; ++numWords)
			;
	numWords >>= 1;
}

const char *cp;
#if 1
UInt8
strLen(const char *cp)
{
	UInt8 rval = 0;
	for (;*cp;++cp,++rval);
	return rval;
}
#endif

UInt8 w = 0;
UInt8 rep = 1;
void
putWords()
{
	//UInt8	w = 0;
	UInt8	ts;
	char	pos;
	UInt8	ws;
	const char	*cp;

	if ( !--rep ) { 
		w = pseudoRand(0,numWords);	
		rep = pseudoRand(1,10);
	}
	ts = pseudoRand(0,4);
	cp = words[w];
	pos = *cp++;
	ws = pseudoRand(0,8-strLen(cp));
	lcdSetCursor(0,1);
	lcdSendMessage("                ");
	lcdSetCursor(pos == '+' ? ts : ws,1);
	lcdSendMessage(pos == '+' ? "time" : cp);
	lcdSetCursor((pos == '-' ? ts : ws)+8,1);
	lcdSendMessage(pos == '-' ? "time" : cp);
}




