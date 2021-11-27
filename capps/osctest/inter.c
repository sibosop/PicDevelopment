
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#ifdef USE_SERIAL
#include "serial.h"
#endif



void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( TMR1IF )
	{
		TMR1IF = 0;
		TMR1H = 0x80;
#ifdef USE_TIMER_CALLBACK
			timerCallback();
#endif
			oneKhz = 1;
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
		T0IE = 1;	// enable timer 0 interrupt
	}
	if ( rb0Flag ) INTE = 1;
}
