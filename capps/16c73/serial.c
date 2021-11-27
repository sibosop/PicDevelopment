
#include <pic.h>
#include <sys.h>
#include "serial.h"

UInt8	headOfWrite;
UInt8	tailOfWrite;
UInt8	headOfRead;
UInt8	tailOfRead;

Boolean	CharReady;




#define INIT_QUEUE(name) \
	headOf##name = tailOf##name = 0;

void
initQueues(void)
{
	INIT_QUEUE(Write);
	INIT_QUEUE(Read);
}

#define DEQUEUE(name) \
UInt8 \
dequeue##name (UInt8 *c) \
{ \
	if ( headOf##name == tailOf##name ) return 0; \
	*c = buffOf##name [headOf##name]; \
	++( headOf##name ); \
	if ( headOf##name == QueueSize##name ) headOf##name = 0; \
	return 1; \
}


#define ENQUEUE(name) \
void \
enqueue##name (UInt8 c) \
{ \
	buffOf##name [tailOf##name] = c; \
	++ tailOf##name ; \
	if ( tailOf##name == QueueSize##name ) tailOf##name = 0; \
}

DEQUEUE(Write)
ENQUEUE(Write)

DEQUEUE(Read)
ENQUEUE(Read)



void
initSerial(UInt8 rFlag, UInt8 tFlag)
{
	initQueues();
	SPBRG = baudMidi;
	SYNC = 0;	// turn of sync mode (makes asynch)
	SPEN = 1;	// enable serial port
	if ( rFlag )
	{
		CREN = 1;	// enable recv
		RCIE = 1;	// enable receive interrupt
	}
	else
	{
		CREN = 0;
		RCIE = 0;	// enable receive interrupt
	}

	if ( tFlag )
	{
		TXIE = 0;	// enable transmit interrupt
		TXEN = 1;	// enable transmission
	}
	else
	{
		TXIE = 0;	// enable transmit interrupt
		TXEN = 0;	// enable transmission
	}
}

Boolean ready = 0;
void
checkTransmit(void)
{
#if 1
	if ( !TXIE ) return;
	while ( TXIF )
	{
	    UInt8 c;
		UInt8 r = dequeueWrite(&c);
		if ( !r )
		{
			TXIE = 0;
			return;
		}
		TXREG = c;
	}
#else
	while ( TXIF )
	{
		if ( ready )
			TXREG = 0xFE;
		else
			break;
	}
	ready = 0;
	TXIE = 0;
#endif
}


void
checkReceive(void)
{
	UInt8 c;
	while ( RCIF )
	{

		// check for errors and reset
		// transmitter
		if ( OERR || FERR )
		{
			CREN = 0;
			CREN = 1;
			return;
		}
		c = RCREG; 
		enqueueRead(c);
		CharReady = 1;
	}
}

void
writeChar(UInt8 c)
{
	di();
#if 1
	enqueueWrite(c);
	TXIE = 1;
#else
	ready = 1;
#endif
	ei();
}

UInt8
readChar(UInt8 *c)
{
	UInt8 rval;
	di();
	rval = dequeueRead(c);
	ei();
	return rval;	
}

	
