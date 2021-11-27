#include <pic.h>
#include <sys.h>
#include "stepper.h"

const int lookup[] = { 1,4,2,8 };

void
stepOn(UInt8 motor, UInt8 s)
{
	s = lookup[s];
	if ( motor )
		s <<= 4;
	PORTC |= s;
}

void
stepOff(UInt8 motor, UInt8 s)
{
	s = lookup[s];
	if ( motor )
		s <<= 4;
	PORTC &= ~s;
}

void	
stepStop(UInt8 motor){
	if ( motor == 0 ) {
		PORTC &= 0xf0;
	} else {
		PORTC &= 0x0f;
	}
}
