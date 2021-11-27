#include <pic.h>
#include <sys.h>
#include "types.h"
#include "timers.h"
#include "interrupt.h"



DECLARE_TIMERS(4);
DECLARE_INT_FLAGS;

__CONFIG(UNPROTECT & XT & PWRTEN & BORDIS & WDTEN);
//
// Here's the hookup
// looking at the pcb board with the top left side being the
// "bottom"
// RB0,RB1,AD1	- Right Front Motor
// RB2,RB3,AD2  - Left  Front Motor
// RB4,RB5,AD3	- Left  Back  Motor
// RB6,RB7,AD0  - Right Back  motor
//
// Notice that Ad0 goes to the right back




enum {
	MOTORRB = 0,
	MOTORRF, 
	MOTORLF,
	MOTORLB
};

// The order of these doesn't matter
//
typedef enum {
	RB,
	RF,
	LF,
	LB
} Motor ;


typedef enum {
	CLOCKWISE = 0,
	COUNTER_CLOCKWISE = 1
} Direction;
	

#define RF_MASK	0x03
#define	RFF	0x01
#define RFB	0x02

#define LF_MASK	0x0C
#define	LFF	0x04
#define LFB	0x08

#define LB_MASK	0x30
#define	LBF	0x10
#define LBB	0x20

#define RB_MASK	0xC0
#define	RBF	0x40
#define RBB	0x80




TWEEK_MILLI(1,99);

UInt8 pulseCount = 0;
UInt8 motorState = 0;
UInt8 sideFlag   = 0;
Boolean RFenable;
Boolean LFenable;
Boolean RBenable;
Boolean LBenable;

stop()
{
 RFenable   = 0;
 LFenable   = 0;
 RBenable   = 0;
 LBenable   = 0;
}
start()
{
 RFenable   = 1;
 LFenable   = 1;
 RBenable   = 1;
 LBenable   = 1;
}
void
init()
{
	TRISA = 0x00;	// RA4 output, RA0-3 inputs (for analog) 
	TRISB = 0x00;	// PORT B is all outputs
	OPTION = 0x8F;	// disable pullups
	PORTB = 0;
	PORTA = 0;
	pulseCount = 0;
	motorState = 0;
	start();
}

#define CYCLE_SHIFT	4
#define CYCLE_COUNT	16

UInt8
pulseOff(UInt8	i, UInt8 mask)
{
	switch ( i )
	{
		case MOTORRF:
			mask &= ~RF_MASK;
			break;
		
		case MOTORLF:
			mask &= ~LF_MASK;
			break;

		case MOTORLB:
			mask &= ~LB_MASK;
			break;

		case MOTORRB:
			mask &= ~RB_MASK;
			break;
	}
	return mask;
}

UInt8
pulseOn(UInt8 i,UInt8 mask)
{
	switch ( i )
	{
		case MOTORRF:
			if ( RFenable )
			{
				mask &= ~RF_MASK;
				mask |= motorState & RF_MASK;
			}
			break;
		
		case MOTORLF:
			if ( LFenable )
			{
				mask &= ~LF_MASK;
				mask |= motorState & LF_MASK;
			}
			break;

		case MOTORLB:
			if ( LBenable )
			{
				mask &= ~LB_MASK;
				mask |= motorState & LB_MASK;
			}
			break;

		case MOTORRB:
			if ( RBenable )
			{
				mask &= ~RB_MASK;
				mask |= (motorState & RB_MASK);
			}
			break;
	}
	return mask;
}


void
checkPulser()
{
	UInt8	i;
	// us mask to avoid glitch pulse on always on state
	UInt8 	mask = PORTB;
	if ( ++pulseCount == CYCLE_COUNT )
	{
		pulseCount = 0;
		sideFlag = sideFlag ? 0 : 1;
	}
	for ( i = 0; i < 4; ++i )
	{
		UInt8 ad = 0xFF >> CYCLE_SHIFT;
		if (!pulseCount)
		{
			if ( sideFlag )
				mask = pulseOn(i,mask);
			else
				mask = pulseOff(i,mask);
			
		}
		if ( pulseCount == ad )
		{
			mask = pulseOff(i,mask);
		}
	}
	PORTB = mask;
}


void
motorOn(Motor motor, Direction direction)
{
	switch ( motor )
	{
		case RF:
			motorState &= ~RF_MASK;
			motorState |= (direction == CLOCKWISE) ? RFF : RFB;
			break;

		case LF:
			motorState &= ~LF_MASK;
			motorState |= (direction == CLOCKWISE) ? LFF : LFB;
			break;

		case LB:
			motorState &= ~LB_MASK;
			motorState |= (direction == CLOCKWISE) ? LBF : LBB;
			break;

		case RB:
			motorState &= ~RB_MASK;
			motorState |= (direction == CLOCKWISE) ? RBF : RBB;
			break;
	}
	
}

void
forward()
{
	motorOn(RF,CLOCKWISE);
	motorOn(RB,CLOCKWISE);
	motorOn(LF,COUNTER_CLOCKWISE);
	motorOn(LB,COUNTER_CLOCKWISE);
}

void
backward()
{
	motorOn(RF,COUNTER_CLOCKWISE);
	motorOn(RB,COUNTER_CLOCKWISE);
	motorOn(LF,CLOCKWISE);
	motorOn(LB,CLOCKWISE);
}
void
sideforward()
{
	motorOn(RF,CLOCKWISE);
	motorOn(LF,CLOCKWISE);
	motorOn(RB,COUNTER_CLOCKWISE);
	motorOn(LB,COUNTER_CLOCKWISE);
}

void
sidebackward()
{
	motorOn(RF,COUNTER_CLOCKWISE);
	motorOn(LF,COUNTER_CLOCKWISE);
	motorOn(LB,CLOCKWISE);
	motorOn(RB,CLOCKWISE);
}

void
counterclockwise()
{
	motorOn(RF,COUNTER_CLOCKWISE);
	motorOn(RB,COUNTER_CLOCKWISE);
	motorOn(LF,COUNTER_CLOCKWISE);
	motorOn(LB,COUNTER_CLOCKWISE);
}
void
clockwise()
{
	motorOn(RF,CLOCKWISE);
	motorOn(RB,CLOCKWISE);
	motorOn(LF,CLOCKWISE);
	motorOn(LB,CLOCKWISE);
}



main()
{
	UInt8 direction = 0;
	init();
	initInterrupt(0,1);
	initTimers();
	ei();
	
	// startup wait loop
	PORTB = 0;
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz ) 
		{
			oneKhz = 0;
			checkTimers();
			checkPulser();
			if ( timerReady(0) )
			{
				setTimer(0,4500);
				if ( ++direction > 5 )
					direction = 0;
				
				switch ( direction )
				{
					case 0:
						start();
						sidebackward();
						break;

					case 1:
						sideforward();
						break;

					case 2:
						counterclockwise();
						break;

					case 3:
						forward();
						break;

					case 4:
						backward();
						break;


					case 5:
						clockwise();
						break;
				}
			}
		}
	}
}

