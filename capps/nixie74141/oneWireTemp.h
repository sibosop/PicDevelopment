#ifndef oneWireTemp
#define oneWireTemp
#include "types.h"

#define ONE_WIRE_TIMER	2
#define ONE_WIRE_PORT   RA1
#define ONE_WIRE_TRIS   TRISA1
#define ONE_WIRE_DEBUG	RA1
#define ONE_WIRE_DEBUG_TRIS	TRISA1

extern void checkOneWireTemp();
extern void initOneWireTemp();
extern UInt16 cval;
extern UInt16 fval;
#endif
