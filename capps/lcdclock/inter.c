
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
	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1IF = 0;
		++pinInt;
	}
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
#if 0
	if ( INTF )
	{
		INTF = 0;
		pinInt = 1;
	}
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
	
	if ( rb0Flag ) 
	{
		pinInt = 0;
		TMR1IE = 1;
		T1CON = 0xE;
		TMR1ON = 1;
		TMR1L = 0;
		TMR1H = 0x80;
	}	
	
}
