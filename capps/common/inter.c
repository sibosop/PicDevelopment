
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#ifdef USE_SERIAL
#include "serial.h"
#endif


UInt8	milli = 1;


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
#ifdef USE_TIMER_CALLBACK
			timerCallback();
#endif
			milli = milliCount;
			oneKhz = 1;
		}
	}
	if ( INTF )
	{
#ifdef USE_PIN_CALLBACK
		pinCallback();
#endif
		INTF = 0;
		pinInt = 1;
	}
#ifdef USE_SERIAL
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
		T0IF = 0;
	}
	if ( rb0Flag ) INTE = 1;
	
	
}
