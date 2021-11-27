#include <pic.h>
#include "midi.h"

extern Boolean clockFlag;
void
handleRealTimeClock()
{
	RC5 = 1;
	clockFlag = 1;
}

void
handleNoteOn(void)
{
	UInt8 note = mbyte1 % 8;
	UInt8 vel;
	vel = mbyte2 ? 1 : 0;
	switch ( note )
	{
		default:
		case 0:
			RB0 = vel;
			break; 
		case 1:
			RB1 = vel;
			break; 
		case 2:
			RB2 = vel;
			break; 
		case 3:
			RB3 = vel;
			break; 
		case 4:
			RB4 = vel;
			break; 
		case 5:
			RB5 = vel;
			break; 
		case 6:
			RB6 = vel;
			break; 
		case 7:
			RB7 = vel;
			break; 
	}
}
