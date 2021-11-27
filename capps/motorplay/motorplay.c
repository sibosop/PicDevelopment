#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "callbackTimer.h"
#include "stepper.h"
#include "pseudorand.h"
#include "servo.h"
#include "onerev.h"

DECLARE_INT_FLAGS;
DECLARE_CALLBACK_TIMERS(2);
// test comment 2
// test comment

__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);
void
init()
{
	TRISC = 0x00;	// make C outputs
	TRISB = 0x00;
	ADCON1 = 0x07;	// make register A all digital inputs.
	TRISA = 0x0;	// motor switch inputs on RA1 & RA2
	PORTA = 0;

	PORTB = 0;
	PORTC = 0;
	OPTION = 0x8F;	// divide wdt by 2 disable pullups
	INTEDG = 1;
	TMR1IE = 1;
	T1CON = 1;  // timer one enabled internal interrupt
	TMR1H = 0x80;
	TMR1L = 0;
}





enum opCodes {
	STEP,
	END,
	STOP,
	SUSPEND,
	RESTART,
	WAIT,
	SEQ,
	RET,
	REP,
	ENDREP,
	RANDSEQ,
	RANDWAIT,
	RANDSTEP,
	RANDREP,
	WAITTILL,
	SET,
	ADD,
	SUB
};

enum Functions {
	RAND = 0x80
	,GLOB
	,LOC
};

const UInt16 steps[] = {
	16,15,14,13,12,11,10,9,8,7,6,5,4,3
};

const UInt16 speeds[] = {
	500,1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000
};
#define NUMSPEEDS	((UInt8)(sizeof(speeds)/sizeof(UInt16)))
#define NUMSTEPS	((UInt8)(sizeof(steps)/sizeof(UInt16)))
enum SequenceNames {
	Main0,
	Main1,
	Seq0,
	Seq1,
	Seq2,
	Seq3,
	MaxSeq,
};


const UInt8 main0[] = {
	SET,GLOB,0,RAND,15,30,
	WAIT,0,
	SEQ,Seq0,
	WAITTILL,
	SET,GLOB,0,RAND,4,8,
	SEQ,Seq1,
	WAITTILL,
	REP,5,
	SEQ,Seq2,
	ENDREP,
	WAITTILL,
	END,
};

const UInt8 main1[] = {
	SEQ,Seq0,
	WAITTILL,
	WAIT,1,
	SEQ,Seq1,
	WAITTILL,
	REP,5,
	WAIT,0,
	SEQ,Seq2,
	ENDREP,
	WAITTILL,
	END,
};

const UInt8	seq0[] = {
	REP,GLOB,0,
	STEP,RAND,0,NUMSTEPS,0,
	WAIT,0,
	ENDREP,
	RET,
};

const UInt8	seq1[] = {
	REP,GLOB,0,
	SET,LOC,0,13,
	SET,LOC,1,11,
	REP,11,
	STEP,LOC,0,0,
	STEP,LOC,1,0,
	SUB,LOC,0,1,
	SUB,LOC,1,1,
	WAIT,1,
	ENDREP,
	ENDREP,
	RET,
};

const UInt8 seq2[] = {
	REP,5,
	REP,2,
	STEP,RAND,10,14,RAND,1,2,
	WAIT,0,
	ENDREP,
	STEP,6,1,
	ENDREP,
	RET,
};

const UInt8 seq3[] = {
	REP,20,
	STEP,RAND,10,NUMSTEPS,1,
	WAIT,0,
	ENDREP,
	RET,
};

const UInt8 *seqs[] = {
	main0,
	main1,
	seq0,
	seq1,
	seq2,
	seq3,
};

#define NUM_LOCS_PER_MOTOR	5
typedef struct _playerState {
	UInt8  index;
	UInt8  stopped;
	UInt8	sequence;
	UInt8	stack[16];
	UInt8	sp;
	UInt8	vars[NUM_LOCS_PER_MOTOR];
} PlayerState;



#define PLAYER_BANK	bank2
#define NUM_PLAYERS	2
PLAYER_BANK PlayerState	players[NUM_PLAYERS];
PLAYER_BANK	UInt8 waitTillCount;

#define VAR_BANK	bank3
#define NUM_GLOBS	10

VAR_BANK	UInt8	globs[NUM_GLOBS];
const UInt8 *sqp;
UInt8 index;

UInt8
GetVal(UInt8 m) {
	UInt8 rval = sqp[index++];
	switch ( rval ) {
		default:
			break;
			
		case RAND: {
			UInt16 min = sqp[index++];
			UInt16 max = sqp[index++];
			rval = pseudoRand(min,max);
		}
		break;
		
		case GLOB: 
			rval = globs[sqp[index++]];
		break;
		
		case LOC:
			rval = players[m].vars[sqp[index++]];
		break;
	}
	return rval;
}

void
initPlayers() {
	UInt8 i;
	waitTillCount = 0;
	for ( i = 0; i < 2; ++i ) {
		players[i].index = 0;
		players[i].sp = 0; 
		players[i].sequence = i;
		players[i].stopped = 0;	
	}
}

void play(UInt8 m) {


	PLAYER_BANK UInt8	*st;
	UInt8 	sp;
	UInt8 done = 0;
	
	
	if ( players[m].stopped )
		return;

	index = players[m].index;
	st = players[m].stack;
	sp = players[m].sp;
	sqp = seqs[players[m].sequence];
	while ( !done ) {
		done = 1;
		switch ( sqp[index++] ) {
			case STOP:
			default:
				players[m].stopped = 1;
				suspendCallbackTimer(m);
			break;
			
			case SET: {
				UInt8 glob = sqp[index++];
				UInt8 val = GetVal(m);
				if ( glob == GLOB )
					globs[val] = GetVal(m);
				else
					players[m].vars[val] = GetVal(m);
				done = 0;
			}
			break;
			
			case ADD: {
				UInt8 glob = sqp[index++];
				UInt8 val1 = GetVal(m);
				done = 0;
				if ( glob == GLOB )
					globs[val1] += GetVal(m);
				else
					players[m].vars[val1] += GetVal(m);
			}
			break;
			
			case SUB: {
				UInt8 glob = sqp[index++];
				UInt8 val1 = GetVal(m);
				UInt8 val2 = GetVal(m);
				done = 0;
				if ( glob == GLOB )
					globs[val1] -= val2;
				else
					players[m].vars[val1] -= val2;
			}
			break;
		
			case WAITTILL:
				if ( ++waitTillCount < NUM_PLAYERS ) {
					suspendCallbackTimer(m);
					stepperSpeed(m,0);
				} else {
					int j;
					for ( j = 0; j < NUM_PLAYERS; ++j ) {
						releaseCallbackTimer(j);
					}
					waitTillCount = 0;
				}
				break;

			case RANDSEQ: {
				UInt16 min = GetVal(m);
				UInt16 max = GetVal(m);
				UInt8 seq;
				done = 0;
				st[sp++] = players[m].sequence;
				seq = pseudoRand(min,max);
				players[m].sequence = seq;
				st[sp++] = index;
				index = 0;
				sqp = seqs[players[m].sequence];
				break;
			}
			
			case SEQ:
				done = 0;
				st[sp++] = players[m].sequence;
				players[m].sequence = GetVal(m);
				st[sp++] = index;
				index = 0;
				sqp = seqs[players[m].sequence];
				break;

			case RET:
				done = 0;
				index = st[--sp];
				players[m].sequence = st[--sp];
				sqp = seqs[players[m].sequence];
				break;

			case REP: {
					UInt8 numReps = GetVal(m);
					st[sp++] = index;
					st[sp++] = numReps;
					done = 0;
			}
				break;
			case RANDREP: {
				UInt16 minReps = GetVal(m);
				UInt16 maxReps = GetVal(m);
				UInt8 numReps = pseudoRand(minReps,maxReps);
				st[sp++] = index;
				st[sp++] = numReps;
				done = 0;
			}
				break;	
			case ENDREP: {
				UInt8 reps = st[--sp];
				done = 0;
				if ( --reps ) {
					index = st[sp-1];  // leave on stack
					st[sp++] = reps;
				} else {
					--sp; // remove i
				};
			}
				break;

			case END:
				index = 0;
				done = 0;
			break;

			case SUSPEND:
				suspendCallbackTimer(m);
				stepperSpeed(m,0);
			break;

			case WAIT:
				stepperSpeed(m,0);
				setCallbackTimer(m,speeds[GetVal(m)]);
			break;
				
			case RANDWAIT: {
				UInt16 min = GetVal(m);
				UInt16 max = GetVal(m);
				UInt8 wait = pseudoRand(min,max);
				stepperSpeed(m,0);
				setCallbackTimer(m,speeds[wait]);
				break;
			}

			case RESTART:
				releaseCallbackTimer(GetVal(m));
				done = 0;
				break;
			case RANDSTEP: {
				UInt16 smin = GetVal(m);
				UInt16 smax = GetVal(m);
				UInt16 wmin = GetVal(m);
				UInt16 wmax = GetVal(m);
				UInt8 sp = pseudoRand(smin,smax);
				UInt8 wt = pseudoRand(wmin,wmax);
				stepperSpeed(m,steps[sp]);
				setCallbackTimer(m,speeds[wt]);
				break;
			}
			case STEP: {
				UInt8 step = GetVal(m);
				UInt8 speed = GetVal(m);
				stepperSpeed(m,steps[step]);
				setCallbackTimer(m,speeds[speed]);
			}
			break;
							
		};
	};
	players[m].sp = sp;
	players[m].index = index;

}



UInt8 down = 1;

const UInt16 speedChoices[] = {
	12,10,9,
};
const scSize = sizeof(speedChoices)/sizeof(UInt16);



void
main()
{
	UInt16	servoCounter = 0;
	//UInt8	pos = FULL_LEFT;
	UInt16	wait = 1000;
	UInt8	i= 0;
	UInt8	j;

	init();
	initInterrupt(0,1);
	//initCallbackTimers();
	//initSteppers();
	//initPlayers();
	pseudoSeed(41);
	//initServo();
	initOneRev();
	

	ei();	// enable interrupts

#if 0	
	stepperDir(0,1);
	stepperDir(1,1);
	timerCallback(0,play);
	timerCallback(1,play);
#endif
	while ( 1 ){
		CLRWDT();
		if ( oneKhz ){
			oneKhz = 0;
			//checkCallbackTimers();
			//checkSteppers();
			checkOneRev();
#if 0
			if ( ++servoCounter == wait ) {
				servoCounter = 0;
				startOneRev(i);
				if ( ++i == NUM_ONE_REVS )
					i = 0;
			}
#endif
				
#if 1			
			if ( ++servoCounter == wait ) {
				servoCounter = 0;
				wait = 1000  + pseudoRand(1,6) * 500;
				j = pseudoRand(0,5);
				for ( i = 0; i < j; ++i )
					startOneRev(pseudoRand(0,NUM_ONE_REVS));
			}
#endif
#if 0
			if ( ++servoCounter == wait ) {
				servoCounter = 0;
				wait = pseudoRand(1,3) * 500;
				i = pseudoRand(0,NUM_SERVOS);
				pos = pseudoRand(FULL_LEFT,FULL_RIGHT);
				moveServo(i,pos);	
			}
#endif
		}	
	}
}
