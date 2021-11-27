#ifndef PLAYNOTE_H
#define PLAYNOTE_H
#include "types.h"

typedef struct {
	UInt8	note;
	UInt8	time;
} Note;

extern const UInt8	numNotes;
extern const UInt8	pnChannel;
extern void playNote(UInt8 note, UInt8 vel, UInt8 time);
extern void checkNotes(void);
extern bank1 Note	noteList[]; 


#define DECLARE_NOTES(NUM_NOTES,CHANNEL) \
	const UInt8	pnChannel  = CHANNEL; \
	const UInt8	numNotes = NUM_NOTES; \
	bank1 Note noteList[NUM_NOTES]

#endif
