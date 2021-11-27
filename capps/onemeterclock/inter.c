
#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#include "onemeterclock.h"
#if defined(VERSION_5)
#include "servo.h"
#endif

#ifdef USE_SERIAL
#include "serial.h"
#endif
//void doClock();

UInt8	milli;
void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1IF = 0;
		pinInt = 1;
	}
	if ( T0IF )	{	
		TMR0 = millitCount;
		T0IF = 0;
		
		if ( !(--milli) )
		{
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
