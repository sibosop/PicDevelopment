
#include "types.h"
#include "pic.h"
#include "random.h"
#include "timers.h"



#define MILLIT	104


#define LOW_SET 	8
#define HIGH_SET 	38
#define APP_TIMER	1
#define NUM_SERVOS	6



UInt8	duties[NUM_SERVOS];
UInt8	dutyTimers[NUM_SERVOS];
UInt8 	oneKhz;
UInt16 	periodTimer;
UInt8	servoCnt;
UInt8	downCount;

void interrupt 
interruptHandler()
{
	// test timer 0 interrupt
	if ( T0IF )
	{
		T0IF = 0;
		TMR0 = MILLIT;
		oneKhz = 1;
		if ( !(--periodTimer)  )
		{
			downCount = 0;
			periodTimer = 304;
			PORTB |= (1 << NUM_SERVOS) - 1;	
			for ( servoCnt = 0; servoCnt < NUM_SERVOS; ++servoCnt )	
				dutyTimers[servoCnt] = duties[servoCnt];
		}
		else
		{
			if ( downCount != NUM_SERVOS )
			{
				for ( servoCnt = 0; servoCnt < NUM_SERVOS; ++servoCnt )
				{
					if ( (PORTB & (1<<servoCnt)) )
					{
						if (!(--dutyTimers[servoCnt]))
						{
							++downCount;
							PORTB &= ~(1 << servoCnt);
						}
					}
				}
			}
		}
	}
	
#ifdef USE_SERIAL
	checkTransmit();
	checkReceive();
#endif
}

void
initInterrupt()
{
	TMR0 = MILLIT;
	T0IE = 1;	// enable timer 0 interrupt
	periodTimer = 1;
}

void
init()
{

	PORTC = 0;
	TRISC = 0x0f;	// make C inputs 0-3 and Rcv, 
	PORTB = 0;
	TRISB = 0x00;	// B is all outputs
	PORTA = 0;
	TRISA = 0xb;	// 0,1,3 a-d , 2,4,5 outputs 
	
	OPTION = 0x80;	// divide timer0 by 2 disable pullups
	PEIE = 1;		// enable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	oneKhz = 0;
	ADCON0 = 0;
	ADCON1 = 6;		// set all a/d ports to digital
}


main()
{ 
	UInt8 servo;
	UInt8 duty;
	
	UInt16 appTimer;

	init();
	initInterrupt();
	//initSerial(0,1);

	initRandom();
	//initA2D();
	ei();	// enable interrupts
	
	for (servo = 0; servo < NUM_SERVOS; ++servo )
		duties[servo] = LOW_SET;

	duty = LOW_SET;
	
	appTimer = 1;

	while ( 1 )
	{

		CLRWDT();

		if ( oneKhz )
		{
			oneKhz = 0;	
			if (!(--appTimer) )
			{
				
				appTimer = 4000;
				servo = random(0,NUM_SERVOS);
				duty = random(LOW_SET,HIGH_SET);
				duties[servo] = duty;	
			}
		}
	}
}
