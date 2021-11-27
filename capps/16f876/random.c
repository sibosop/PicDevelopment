#include <pic.h>
#include <sys.h>
#include "random.h"

persistent bank1 UInt16 ranval;
Boolean last;

void
initRandom()
{
	mixRandom();
	mixRandom();
	mixRandom();
	mixRandom();
}

void
mixRandom()
{
	ranval <<= 4;

#if defined(_16C73B) || defined(_16F876)
	ranval |= PORTC & 0xf;
#else
	ranval |= (PORTB >> 4) & 0xf;
#endif
	last = last ? 0 : 1;
	if ( last ) ranval ^= 0xffff;
}

UInt16
random(UInt16 low, UInt16 high)
{
	if ( low > high ) return 0;
	mixRandom();
	return ((ranval % (high -low)) + low);
}
