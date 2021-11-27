#include "midi.h"

void handleSystemExclusive (void){}
void handleSongSelect (void){}
void handlePitchWheel (void){}
void handleRealTimeActiveSensing (void){}
void dohandleEOX (void){
	inSystemCommon = 0;
	handleEOX();
}
void handleEOX(void){}
void handleRealTimeReset (void){}
void handleTuneRequest (void){}
void handleMidiTimeCode (void){}
void handleUndef (void){}
void handleNoteOff (void){}
void handleSongPosition (void){}
