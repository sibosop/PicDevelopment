#include <pic.h>
#include <sys.h>
#include "stepper.h"

void
stepOn(UInt8 motor, UInt8 s)
{
	s += 4*motor;
	if ( s < 6 )
	{
		PORTB |= 1 << s;
	}
	else
	{
		PORTC |= 0x10 << (s-6);
	}
}

void
stepOff(UInt8 motor, UInt8 s)
{
	s += 4*motor;
	if ( s < 6 )
	{
		PORTB &= ~(1<<s);
	}
	else
	{
		PORTC &= ~(0x10<<(s-6));
	}
}

void	
stepStop(UInt8 motor){
	if ( motor == 0 ) {
		PORTB &= 0xf0;
	} else {
		PORTB &= 0xCF;
		PORTC &= 0xCF;
	}
}
