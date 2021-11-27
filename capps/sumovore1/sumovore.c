#include <pic.h>
#include <sys.h>
#include "sumosup.h"
#include "interrupt.h"
#include "a2d.h"
DECLARE_INT_FLAGS;


#define MOTOR_DEBOUNCE_TIME	1000
#define	BLACK_THRESHOLD		190
#define RETREAT_TIME		500


TWEEK_MILLI(10,99);
main()
{
	UInt16	ledTimer = 0;
	UInt16	motorTimer = 0;
	UInt16	retreatTimer = 0;	
	initSumo();
	initA2D();
	initInterrupt(0,1);
	clearLeds();
	ei();
	SUMO_GO;
	while ( 1 )
	{
		checkA2D();
		if ( oneKhz )
		{
			RC5 = RC5 ? 0 : 1;
			
			checkIr();
			
			oneKhz = 0;
			if ( ++ledTimer > 1000 )
			{
				ledTimer = 0;
				LED0 = LED0 ? 0 : 1;
			}
			if ( retreatTimer == 0 )
			{
				if ( (A2DVals[0] > BLACK_THRESHOLD))
				{
					retreatTimer = RETREAT_TIME;
					RIGHT_MOTOR_BACKWARD;
					LEFT_MOTOR_FORWARD;
					continue;
				}
				else if ( (A2DVals[4] > BLACK_THRESHOLD) )
				{
					retreatTimer = RETREAT_TIME;
					RIGHT_MOTOR_BACKWARD;
					LEFT_MOTOR_FORWARD;
					continue;
				}
			}
			else
			{
				if ( --retreatTimer == 0 )
				{
					SUMO_FORWARD;
				}
				continue;
			}
			
#if 1
			if ( motorTimer != 0 )
				--motorTimer;
			if( irOn(IRL) )
			{
				RIGHT_MOTOR_FORWARD;
			}
			else
			{	
				if ( motorTimer == 0 )
				{
					RIGHT_MOTOR_BACKWARD;
					motorTimer = MOTOR_DEBOUNCE_TIME;
				}
			}
			if ( irOn(IRR) )
			{
				LEFT_MOTOR_FORWARD;
			}
			else
			{
				if ( motorTimer == 0 )
				{
					LEFT_MOTOR_BACKWARD;
					motorTimer = MOTOR_DEBOUNCE_TIME;
				}
			}
#endif	
		} // if ( oneKhz )
	} // while(1)
} // main
