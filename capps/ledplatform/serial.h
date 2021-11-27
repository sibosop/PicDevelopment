#ifndef SERIAL_H
#define SERIAL_H
#include "types.h"

#define CLOCKRATE 20

#if	(CLOCKRATE == 20)
#define BAUD24		129	// 2400
#define BAUD96		32	// 9600
#define BAUD192		15	// 19200
#define BAUDMIDI	9	// 31250 for midi
#elif	(CLOCKRATE == 10)
#define BAUDMIDI	4	// 31250 for midi
#elif	(CLOCKRATE == 4)
#define BAUDMIDI	1	// 31250 for midi
#endif


extern void initSerial(UInt8 r,UInt8 t);
extern void checkTransmit(void);
extern void	checkReceive(void);
extern void writeChar(UInt8 c);
extern UInt8 readChar(UInt8 *c);
extern Boolean CharReady;

#define DISABLE_SERIAL	\
	void checkTransmit(void) {} \
	void checkReceive(void) {}

extern	const UInt8 QueueSizeWrite;
extern	const UInt8 QueueSizeRead;
extern  const UInt8 baudMidi;

#ifdef _16F876
#define QUEUE_BANK	bank3
#else
#define QUEUE_BANK
#endif

extern QUEUE_BANK UInt8 buffOfWrite[];
extern QUEUE_BANK UInt8 buffOfRead[];

#define DECLARE_QUEUES(r,w) \
const	UInt8 QueueSizeWrite  = (w);	\
const	UInt8 QueueSizeRead  = (r); \
QUEUE_BANK UInt8	buffOfWrite[(w)]; \
QUEUE_BANK UInt8	buffOfRead[(r)]; \
const   UInt8	baudMidi = BAUD96
#endif
