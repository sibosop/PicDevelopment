
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#if defined(_16C73B) || defined(_16F876)
#include "serial.h"
#endif


UInt8	milli;

void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = millitCount;
		if ( !(--milli) )
		{
			milli = milliCount;
			oneKhz = 1;
		}
	}
	if ( INTF )
	{
		INTF = 0;
		pinInt = 1;
	}
#if defined(_16C73B) || defined(_16F876)
	checkTransmit();
	checkReceive();
#endif
}

void
initInterrupt(UInt8 rb0Flag,UInt8 timerFlag)
{
	if ( timerFlag )
	{
		milli = milliCount;
		T0IE = 1;	// enable timer 0 interrupt
	}
	if ( rb0Flag ) INTE = 1;
	
	
}
