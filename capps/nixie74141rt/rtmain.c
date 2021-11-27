#include <pic.h>
#include <sys.h>
#include "types.h"
#include "rtclock.h"
#include "rtcbutton.h"
#include "seg.h"
#include "lcd.h"
#include "picpwm.h"
#include "worddisp.h"
#include "pseudorand.h"
extern void displayTime();
#define TICK_LOW	0
#define TICK_RATE	1
SetMode	setMode = Running;
RunMode	runMode;
Boolean	oneKhz;
UInt8 halfSecFlag;
UInt16	secCount;
UInt8 tickDuty= TICK_LOW;
UInt8 tickTimer;
UInt8 secDuty;
UInt8 secTimer;
UInt8 period;

UInt8 servo1Duty;
UInt8 servo1Timer;
UInt8 servo2Duty;
UInt8 servo2Timer;

#define TICK_REG	RC2
#define TICK_TRIS	TRISC2
#define SEC_REG		RC1
#define SEC_TRIS	TRISC1
#define TICK_TIME	5
#define	SERVO1_REG	RA0
#define SERVO1_TRIS	TRISA0
#define SERV02_REG	RA1
#define	SERVO2_TRIS	TRISA1

#define SERVO_LOW	5
#define SERVO_HIGH	50

#define PERIOD	240
UInt8 dummy;

#define QUANT	57

#define PWM_RESET(d,r,t) \
	if ( !d ) { \
		r = 0; \
	} else {\
		r = 1;\
		t = d;\
	}

#define PWM_TEST(d,r,t) \
	if ( r && d <= PERIOD) { \
		if ( !--t ) \
			r = 0; \
	}
	
void interrupt
interruptHandler()
{
#if 1
	if ( T0IF  ) {
		T0IF = 0;
		TMR0 = QUANT;
		if ( !(--period) ) {
			period = PERIOD;
			PWM_RESET(tickDuty,TICK_REG,tickTimer);	
			PWM_RESET(secDuty,SEC_REG,secTimer);
			PWM_RESET(servo1Duty,SERVO1_REG,servo1Timer);
			PWM_RESET(servo2Duty,SERV02_REG,servo2Timer);
		} else {
			PWM_TEST(tickDuty,TICK_REG,tickTimer);
			PWM_TEST(secDuty,SEC_REG,secTimer);
			PWM_TEST(servo1Duty,SERVO1_REG,servo1Timer);
			PWM_TEST(servo2Duty,SERV02_REG,servo2Timer);
		}
	}
#endif
	// test timer 0 interrupt
	if ( INTF ) {
		INTF = 0;	
		oneKhz = 1;
		if ( !--secCount ) {
			halfSecFlag = 1;
			secCount = 512;
		}

	}
	if ( SSPIF ) {
		SSPIF = 0;
		if ( !stopFlag && STAT_BF )
			rtcInterrupt();
	}
}


void
init()
{
	PORTC = 0;
	TRISC = 0x04;	// make C outputs, bit 2 data read
	TRISB = 0x00;
	TRISA = 0x00;
	PORTA = 0;
	PORTB = 0;
	OPTION = 0x80;	// divide wdt by 2 disable pullups
	PEIE = 1;		// disable extra peripheral interrupt
	SSPEN = 0;		// disable sync serial port
	INTEDG = 1;
	ADCON1 = 0x7;
	ADCON0 = 0;
	SPEN = 0;	
	secCount = 512;
	TICK_TRIS = 0;
	SEC_TRIS = 0;
	period = PERIOD;
	secDuty = 0;
	tickDuty = 0;
	T0IE = 1;
	T0IF = 0;
	SERVO1_TRIS = 0;
	SERVO2_TRIS = 0;

}


Boolean up;
Boolean lcdInitDone = 0;

const char * const msg = "hello";
void
main()
{
	UInt8 tickCount = TICK_TIME;
	UInt8 lastSec;
	init();
	rtcInit();
	segInit();
	initButtons();
	initWordDisp();
	//initPwm();
	ei();	// enable interrupts
	rtcStart();
	setMode = Running;
	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{	
			oneKhz = 0;
			checkButtons();
			if ( !lcdInitDone ) {
				lcdInitDone = checkLcd();
				if ( lcdInitDone ) {
					pseudoSeed(rtcGetSecond());
				}
			}
			if ( !--tickCount ) {
				tickCount = TICK_TIME;
				if ( up ) {
					if ( tickDuty != PERIOD )
						tickDuty++;
				} else {
					if ( tickDuty != 0 ) 
						tickDuty--;
				}		
			}
		}
		if ( halfSecFlag ) {
			halfSecFlag =0;
			if ( rtcGetSecond() != lastSec ) {
				up = up ? 0 : 1;
				if ( up ) {
					servo1Duty = SERVO_HIGH;
					servo2Duty = SERVO_HIGH;
				} else {
					servo1Duty = SERVO_LOW;
					servo2Duty = SERVO_LOW;
				}
				lastSec = rtcGetSecond();
				secDuty = bcd2Bin(lastSec) * 4;
				//SET_PWM2(secDuty);
				segDotLo = 1;
				
				if ( lcdInitDone )
				{
					//lcdSetCursor(0,0);	
					displayTime(0);
				}
			} else  {
 				segDotLo = 0;
			}
			rtcStart();
		}	
	}
}

