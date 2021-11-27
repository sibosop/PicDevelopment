
#include "playnote.h"
#include "midi.h"

void
playNote(UInt8 n, UInt8 v, UInt8 t)
{
	UInt8 i;
	for( i = 0; i < numNotes; i++ )
	{
		if (!noteList[i].time)
		{
			putNote(n,v,pnChannel);
			noteList[i].note = n;
			noteList[i].time = t;
			break;
		}
	}
}

void
checkNotes(void)
{
	UInt8 i;
	for( i = 0; i < numNotes; i++ )
	{
		if (noteList[i].time)
		{
			if ( --noteList[i].time == 0 )
			{
				putNote(noteList[i].note,0,pnChannel);
			}
		}
	}
}
