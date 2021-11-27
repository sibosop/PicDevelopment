#ifndef MIDI_H
#define MIDI_H
#include "types.h"
extern void putNote(UInt8 note, UInt8 vel, UInt8 chan);
extern void putCtrl(UInt8 ctrl, UInt8 val, UInt8 chan);
extern void	putPgm(UInt8 pgm, UInt8 chan);
extern void	midiReset(void);

extern bank1 UInt8	mbyte1;
extern bank1 UInt8	mbyte2;
extern bank1 UInt8	mchannel;
extern bank1 UInt8	msystem;
extern bank1 UInt8	mstatus;
extern bank1 UInt8 msong;
extern bank1 UInt8	msptr1;
extern bank1 UInt8	msptr2;
extern bank1 UInt8	mtimec1;
extern bank1 UInt8 mtimec2;
extern bank1 UInt8	mtest;
extern bit inSystemCommon;

#define MIDINOTEON 0x90
#define	MIDIPROGRAMCHANGE 0xC0
#define MIDICMODE 0xB0

void checkmparse(void);
void initmparse(void);
void handleChannelPressure (void);
void handleNoteOn (void);
void handleSystemExclusive (void);
void handleSongSelect (void);
void handlePitchWheel (void);
void handleRealTimeActiveSensing (void);
void dohandleEOX (void);
void handleEOX(void);
void handleRealTimeContinue (void);
void handleRealTimeReset (void);
void handlePolyKeyPressure (void);
void handleTuneRequest (void);
void handleMidiTimeCode (void);
void handleUndef (void);
void handleNoteOff (void);
void handleRealTimeStop (void);
void handleControlChange (void);
void handleProgramChange (void);
void handleRealTimeStart (void);
void handleRealTimeStop (void);
void handleRealTimeContinue (void);
void handleRealTimeClock (void);
void handlerInit (void);
void handleSongPosition (void);
#endif
