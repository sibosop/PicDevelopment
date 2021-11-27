#include <pic.h>
#include "types.h"
#include "midi.h"


char test[] =
{
	0xb5,0x55,0x66,0xb3,0x01,0x02	
};

UInt8 index = 0;

UInt8
readChar(UInt8 *c)
{
	UInt8 rval = 0;
	if ( index < sizeof(test) )
	{
		*c = test[index];
		++index;
		rval = 1;
	}
	return rval;
}



void
main()
{
	initmparse();
	while ( 1 )
	{
		CLRWDT();
		checkmparse();
	}
}
