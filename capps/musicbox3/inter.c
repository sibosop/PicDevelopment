
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#ifdef USE_SERIAL
#include "serial.h"
#endif


UInt8	milli;


void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( TMR1IF ) {
		RB0 = RB0 ? 0 : 1;
		TMR1IF = 0;
		TMR1H = 0xFE;
		TMR1L = 0xE8;
	}
	if ( T0IF )
	{
		
		T0IF = 0;
		TMR0 = millitCount;
		if ( !(--milli) )
		{
#ifdef USE_TIMER_CALLBACK
			timerCallback();
#endif
			RB1 = RB1 ? 0 : 1;
			milli = milliCount;
			oneKhz = 1;
		}
	}
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
