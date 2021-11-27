#include <pic.h>
#include <sys.h>
#include "sumosup.h"
#include "interrupt.h"
#include "a2d.h"
DECLARE_INT_FLAGS;


#define MOTOR_DEBOUNCE_TIME	1000
#define	BLACK_THRESHOLD		190
#define RETREAT_TIME		500


TWEEK_MILLI(10,18);
main()
{
	UInt16	ledTimer = 0;
	UInt16	motorTimer = 0;
	//UInt16	speedTimer = 0;
	UInt16	retreatTimer = 0;
	//UInt16	speed = MEDIUM_SPEED;
	//UInt8	speedFlag = 1;	
	initSumo();
	initA2D();
	initInterrupt(0,1);
	clearLeds();
	ei();
	motorSpeed(MEDIUM_SPEED,LEFT_MOTOR);
	motorSpeed(MEDIUM_SPEED,RIGHT_MOTOR);
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
			if ( A2DVals[5] > A2DVals[6] )
			{
				LED1 = 1;
				LED2 = 0;
			}
			else
			{
				LED2 = 1;
				LED1 = 0;
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
	
			if ( motorTimer != 0 )
				--motorTimer;

			if(  irOn(IRM) )
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
#if 0
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
