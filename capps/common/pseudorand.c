#include "pseudorand.h"
UInt16 seed = 1;
void
pseudoSeed(UInt16 s) {
	seed = s;
}


UInt16
pseudoRand(UInt16 l, UInt16 h)
{
	UInt16 range;
    UInt16 r = seed;
    r <<= 1;
    if ( seed & 0x8000 )
        r |= 1;
    if ( seed & 0x2000 )
        r ^= 1;
    if ( seed & 0x1000 )
        r ^= 1;
    if ( seed & 0x400)
        r ^= 1;
    seed = r;
	range = h - l;
	r %= range;
	r += l;
    return r;
}
