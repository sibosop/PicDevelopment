#include <pic.h>
#include <sys.h>
#include "types.h"
#include "sumosup.h"
#include "timers.h"

void
initPwm()
{
	// guarantee the the two pwm ports are outputs
	TRISC1 = 0;
	TRISC2 = 0;
	// no interrupt for timer2
	TMR2IE = 0;
	// turn on timer2
	TMR2ON = 1;
	// set the PWM period
	PR2 = 0xff;
	// timer2 prescaler 4 - 5k -> 01   1 - 20k -> 00  
	T2CKPS1 = 0;
	T2CKPS0 = 0;
	// set PWM mode
	CCP1CON = 0xC;
	CCP2CON = 0xC;

	
}




void
motorSpeed(UInt16 value, UInt8 motor)
{
	if ( motor == RIGHT_MOTOR )
	{
		CCPR1L = (value & 0x3fC) >> 2;
		CCP1CON = ((value & 0x3) << 4) | 0xC;
	}
	else
	{
		CCPR2L = (value & 0x3fC) >> 2;
		CCP2CON = ((value & 0x3) << 4) | 0xC;
	}
} 

