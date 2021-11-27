#include <pic.h>
#include <sys.h>
#include "types.h"


#include "mclock.h"


const UInt8 millitCount = 0;
const UInt8 milliCount = 17;


UInt8	milli = 1;
Boolean	pinInt;
Boolean oneKhz;

UInt8	meterDuty;
UInt8	meterTimer;
UInt8 	periodTimer;

UInt8	hours;
UInt8	seconds;
UInt8	minutes;
Boolean	clockDisable;


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
	
		if ( !(--periodTimer)  )
		{
			periodTimer = PERIOD_HIGH;
			RB0 = 1;
			meterTimer = meterDuty;
		}
		else
		{
			if ( meterTimer ) {
				if ( !--meterTimer )
					RB0 = 0; 
			}
		}	
	}	
	// one second timer interrupt
	if ( TMR1IF )
	{
		TMR1H = 0x80;
		TMR1IF = 0;
		pinInt = 1;
	}
}

void
initInterrupt()
{
	TMR0 = millitCount;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
	pinInt = 0;
	TMR1IE = 1;
	T1CON = 0xE;
	TMR1ON = 1;
	TMR1L = 0;
	TMR1H = 0x80;
}


void
init()
{
	PORTC = 0;
	TRISC = 0x03;	// make C outputs
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	pinInt = 0;
	oneKhz = 0;
}



void
main()
{
	init();
	initInterrupt();
	initButtons();
	initClock();
	ei();	// enable interrupts
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			RC3 = RC3 ? 0 : 1;
			oneKhz = 0;
			checkSpin();
			checkButtons();	
		}
		if (pinInt) {
			RC4 = RC4 ? 0 : 1;
			pinInt = 0;
			doClock();
			updateClock();
		}		
	}
}

