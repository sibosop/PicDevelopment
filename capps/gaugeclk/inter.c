
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#ifdef USE_SERIAL
#include "serial.h"
#endif
void doClock();

UInt8	milli;
void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( TMR1IF )
	{
		TMR1IF = 0;
		TMR1H = 0x80;
		doClock();
	}
	if ( T0IF )
	{
		
		T0IF = 0;
		TMR0 = millitCount;
		if ( !(--milli) )
		{
			milli = milliCount;
			oneKhz = 1;
		//	RC5 = RC5 ? 0 : 1;
		}
	}
}

