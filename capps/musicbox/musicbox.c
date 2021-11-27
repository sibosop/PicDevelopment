#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "timers.h"
#include "random.h"

void stepOn(UInt8 m, UInt8 s);
void stepOff(UInt8 m, UInt8 s);
void bumpStepper(UInt8 m);

DECLARE_TIMERS(3);
DECLARE_INT_FLAGS;


__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);




#define CLOCKWISE	1
#define COUNTER_CLOCKWISE 0;
#define NUM_MOTORS 2
#define PROG_TIMER	NUM_MOTORS
#define TOTAL_SONG_REVS	30
#define SPEED_MULTIPLIER	1
#define MAX_SPEED	7
#define MIN_SPEED	30
#define STEPS_PER_REV 100
#define SWITCH_DEBOUNCE	15
#define STARTUP_WAIT	10000

typedef enum _switchStates {
	CheckSwitch,
	SwitchDebounce
} SwitchState;

typedef enum _progStates {
	PreStartUp,	//sigh
	StartUp,
	StartWait,
	Running,
} ProgState;


typedef struct _MotorState 
{
	UInt8	revCnt;
	UInt8	numRevs;
	UInt16		stepTime;
	UInt8		motorState;
	SwitchState switchState;
	UInt8		dir;
	UInt8		enabled;
	UInt8		num;
	UInt8		startMask;
} MotorState;

MotorState motorState[NUM_MOTORS];
ProgState	progState;

void
init()
{
	TRISC = 0x8f;	// make C inputs 0-3 and Rcv, RC4 is clockout
	TRISB = 0x00;
	ADCON1 = 0x07;	// make register A all digital inputs.
	TRISA = 0x6;	// motor switch inputs on RA1 & RA2
	PORTA = 0;

	PORTB = 0;
	PORTC = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	INTEDG = 1;
}

void
lightOn(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB |= 1 << m;
	}
	else
	{
		PORTC |= 0x10 << (m-6);
	}
}

void
lightOff(UInt8 m)
{
	if ( m < 6 )
	{
		PORTB &= ~(1<<m);
	}
	else
	{
		PORTC &= ~(0x10<<(m-6));
	}
}


void
bumpMotor(MotorState *sp)
{
	lightOff(sp->motorState + (4 * sp->num));
	if ( sp->dir == CLOCKWISE )
	{
		++(sp->motorState);
		if ( sp->motorState ==4 )
			sp->motorState = 0;
		lightOn(sp->motorState + (4 * sp->num));
	}
	else
	{
		--(sp->motorState);
		if ( sp->motorState == 255 )
			sp->motorState = 3;
		lightOn(sp->motorState + (4 * sp->num));
	}
}

void
initMotors()
{
	UInt8 motor;
	for ( motor = 0; motor < NUM_MOTORS; ++motor)
	{
		motorState[motor].num = motor;
		motorState[motor].revCnt = 0;
		motorState[motor].numRevs = 0;
		motorState[motor].stepTime = MAX_SPEED * SPEED_MULTIPLIER;
		motorState[motor].motorState = 0;
		motorState[motor].switchState = CheckSwitch;
		motorState[motor].dir = COUNTER_CLOCKWISE;
		motorState[motor].enabled = 1;
	}
	progState = PreStartUp;
}

void
motorOff(MotorState *sp)
{
	sp->enabled = 0;
	lightOff(sp->motorState + (4 * sp->num));
}

void
main()
{
	UInt8 motor;
	UInt8 enableCount = 0;
	init();
	initInterrupt(0,1);
//	initSerial(1,1);
//	initmparse();
	initTimers();
	initRandom();
	initMotors();

	ei();	// enable interrupts

	while ( 1 )
	{
		CLRWDT();
		if ( oneKhz )
		{
			oneKhz = 0;
			checkTimers();
			switch ( progState )
			{
				case PreStartUp:
					// XXX - hardwired for two motors
					if ( (PORTA & 0x6) == 0x6 )
					{
						progState = StartUp;
					}
					break;

				case StartUp:
					enableCount = 0;
					for (motor = 0; motor < NUM_MOTORS; ++motor )
					{
						if ( motorState[motor].enabled )
							++enableCount;
					}

					if ( !enableCount )
					{
							progState = StartWait;
							setTimer(PROG_TIMER,STARTUP_WAIT);
					}				
					break;

				case StartWait:
					if ( timerReady(PROG_TIMER) )
					{
						progState = Running;
						for ( motor= 0; motor < NUM_MOTORS; ++motor )
						{
							motorState[motor].enabled = 1;
						}	
					}
					break;

				default:
				case Running:
					break;
			}

			for( motor = 0; motor < NUM_MOTORS; ++motor )
			{ 
				if ( !motorState[motor].enabled )
					continue;

				if ( timerReady(motor) )
				{
					setTimer(motor,motorState[motor].stepTime);
					bumpMotor(&motorState[motor]);
					
					switch ( motorState[motor].switchState )
					{
						case SwitchDebounce: 
							if ( ++motorState[motor].revCnt == STEPS_PER_REV )
							{
								motorState[motor].revCnt = 0;
								if ( ++motorState[motor].numRevs == SWITCH_DEBOUNCE )
								{
									motorState[motor].numRevs = 0;
									motorState[motor].switchState = CheckSwitch;
								}
							}
							break;
						case CheckSwitch:
							if ( !(PORTA & (0x2 << motor)))
							{
								if ( progState == StartUp )
									motorOff(&motorState[motor]);
								motorState[motor].switchState = SwitchDebounce;
								motorState[motor].revCnt = 0;
								motorState[motor].numRevs = 0;

								if ( progState == PreStartUp )
									motorState[motor].stepTime = SPEED_MULTIPLIER * MAX_SPEED;
								else
								{
									if ( (progState==Running) && (random(0,7) == 1) )
									{
										progState = PreStartUp;
									}
									motorState[motor].stepTime = SPEED_MULTIPLIER * random(MAX_SPEED,MIN_SPEED);
								}
							}
							break;
					}	
				}	
			}
		}
	}
}
