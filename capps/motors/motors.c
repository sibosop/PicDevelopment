
#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
#include "random.h"

DECLARE_INT_FLAGS;

TWEEK_MILLI(1,150);


__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);


void
init()
{
	PORTC = 0;
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	PEIE = 0;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;


}

#define NUM_MOTORS	8
#define MAX_SPEED	20
UInt8	motorSpeed[NUM_MOTORS];
UInt8	pulseCnt;
DECLARE_TIMERS(NUM_MOTORS);

void
motorOn(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB |= 1 << m;
	}
	else
	{
		PORTC |= 4 << (m-6);
	}
}

void
motorOff(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB &= ~(1<<m);
	}
	else
	{
		PORTC &= ~(4<<(m-6));
	}
}

void
checkPulse()
{
	UInt8 i;
	++pulseCnt;
	if ( pulseCnt > MAX_SPEED )
		pulseCnt = 0;

	if ( !pulseCnt )
	{
		for (i = 0; i < NUM_MOTORS; ++i )
		{
			if ( motorSpeed[i] )
			{
				motorOn(i);
			}
			else
			{
				motorOff(i);
			}	
		}
	}
	else
	{
		for (i = 0; i < NUM_MOTORS; ++i )
		{
			if ( motorSpeed[i] == MAX_SPEED )
				continue;
			if ( pulseCnt == motorSpeed[i] )
			{
				motorOff(i);
			}
		}
	}
}

void
checkTransmit(){}
void checkReceive(){}

void
initMotors()
{
	UInt8 i;
	for ( i = 0; i < NUM_MOTORS; ++i )
		motorSpeed[i] = 0;
	pulseCnt = 0;
}


void
main()
{
	UInt8 motor;

	init();
	initInterrupt(0,1);
	initTimers();
	initRandom();
	ei();	// enable interrupts

	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			checkTimers();
			oneKhz = 0;
		}

		for (motor = 0; motor < NUM_MOTORS; ++motor)
		{
			if ( timerReady(motor) )
			{
				setTimer(motor,random(100,500));
				if ( motor < 6 )
				{
					if ( PORTB & (1 << motor) )
					{
						PORTB &= ~(1<<motor);
					}
					else
					{
						PORTB |= 1 << motor;
					}
				}
				else
				{
					if ( PORTC & (0x10 << (motor-6)))
					{
						PORTC &= ~(0x10<<(motor-6));
					}
					else
					{
						PORTC |= 0x10<<(motor-6);
					}
				}
			}
		}
	}
}
