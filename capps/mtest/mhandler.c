#include "midi.h"

void handleChannelPressure (void){}
void handleSystemExclusive (void){}
void handleSongSelect (void){}
void handlePitchWheel (void){}
void handleRealTimeActiveSensing (void){}
void dohandleEOX (void){
	inSystemCommon = 0;
	handleEOX();
}
void handleEOX(void){}
void handleRealTimeContinue (void){}
void handleRealTimeReset (void){}
void handlePolyKeyPressure (void){}
void handleTuneRequest (void){}
void handleMidiTimeCode (void){}
void handleUndef (void){}
void handleNoteOff (void){}
void handleRealTimeStop (void){}
void handleProgramChange (void){}
void handleRealTimeStart (void){}
void handleSongPosition (void){}
