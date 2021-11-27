

#include "serial.h"
#include "midi.h"

void
putNote(UInt8 note, UInt8 vel, UInt8 chan)
{
	writeChar(MIDINOTEON | (chan & 0xf));
	writeChar(note & 0x7f);
	writeChar(vel  & 0x7f);
}

void
putCtrl(UInt8 num, UInt8 val, UInt8 chan)
{
	writeChar(MIDICMODE | (chan & 0xf));
	writeChar(num);
	writeChar(val);
}

void
putPgm(UInt8 num, UInt8 chan)
{
	writeChar(MIDIPROGRAMCHANGE | (chan & 0xf));
	writeChar(num);
}

void
midiReset(void)
{
	writeChar(0xff);
}
