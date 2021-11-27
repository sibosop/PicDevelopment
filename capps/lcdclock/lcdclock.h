
#ifndef LCDBOARD_H
#define LCDBOARD_H

#define INIT_TIMER	0
#define BUTT_TIMER1 INIT_TIMER
#define BUTT_TIMER2	1	
typedef struct _PhraseStruct {
	UInt8	hour;
	UInt8	minute;
	UInt8	second;
	const char * const phrase;
} PhraseStruct;

extern const PhraseStruct const phrases[];
extern const char * const words[];

#endif
