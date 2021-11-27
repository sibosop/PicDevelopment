#ifndef SEG_H
#define SEG_H

#include <pic.h>
#include <sys.h>
#include "types.h"

void	segInit();
void	segDisplay(UInt16);

#define SEG_CLOCK	RB1
#define SEG_DATA	RB2
#define	SEG_LATCH	RB3

extern UInt8 segDotHi;
extern UInt8 segDotLo;

#endif
