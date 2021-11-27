#include <pic.h>
#include <sys.h>

#include "interrupt.h"
#include "callbackTimer.h"
#include "stepper.h"
#include "pseudorand.h"

DECLARE_INT_FLAGS;
DECLARE_CALLBACK_TIMERS(2);

//__CONFIG(UNPROTECT & HS & WDTEN & PWRTEN & LVPDIS & BORDIS & DEBUGDIS);
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
};

const UInt16 steps[] = {
	50,12,9,8,7
};

const UInt16 speeds[] = {
	500,1000,2000
};

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
	RANDSEQ,	Seq0,MaxSeq,
	END,
};

const UInt8 main1[] = {
	RANDSEQ,	Seq0,MaxSeq,
	END,
};

const UInt8	seq0[] = {
	REP, 4,
	STEP,1,2,
	WAIT,2,
	STEP,2,1,
	WAIT,2,
	ENDREP,
	RET,
};

const UInt8	seq1[] = {
	REP,2,
	WAIT,2,
	STEP,3,2,
	WAIT,1,
	STEP,4,2,
	ENDREP,
	RET,
};

const UInt8 seq2[] = {
	REP, 3,
	STEP,1,4,
	WAIT,3,
	ENDREP,
	RET,
};

const UInt8 seq3[] = {
	WAIT,7,
	RET,
};

const UInt8 *seqs[] = {
	main0,
	main1,
	seq2,
	seq0,
	seq1,
	seq3,
};


typedef struct _playerState {
	UInt8  index;
	UInt8  stopped;
	UInt8	sequence;
	UInt8	stack[16];
	UInt8	sp;
} PlayerState;



#define PLAYER_BANK	bank2
PLAYER_BANK PlayerState	players[2];
void
initPlayers() {
	UInt8 i;
	for ( i = 0; i < 2; ++i ) {
		players[i].index = 0;
		players[i].sp = 0; 
		players[i].sequence = i;
		players[i].stopped = 0;	
	}
}

void play(UInt8 m) {
	UInt8 i;

	PLAYER_BANK UInt8	*st;
	UInt8 	sp;
	UInt8 done = 0;
	const UInt8 *sqp;
	
	if ( players[m].stopped )
		return;

	i = players[m].index;
	st = players[m].stack;
	sp = players[m].sp;
	sqp = seqs[players[m].sequence];
	while ( !done ) {
		done = 1;
		switch ( sqp[i++] ) {
			case STOP:
			default:
				players[m].stopped = 1;
				suspendCallbackTimer(m);
			break;

			case RANDSEQ: {
				UInt16 min = sqp[i++];
				UInt16 max = sqp[i++];
				UInt8 seq;
				done = 0;
				st[sp++] = players[m].sequence;
				seq = pseudoRand(min,max);
				players[m].sequence = seq;
				st[sp++] = i;
				i = 0;
				sqp = seqs[players[m].sequence];
				break;
			}

			case SEQ:
				done = 0;
				st[sp++] = players[m].sequence;
				players[m].sequence = sqp[i++];
				st[sp++] = i;
				i = 0;
				sqp = seqs[players[m].sequence];
				break;

			case RET:
				done = 0;
				i = st[--sp];
				players[m].sequence = st[--sp];
				sqp = seqs[players[m].sequence];
				break;

			case REP: {
				UInt8 numReps = sqp[i++];
				st[sp++] = i;
				st[sp++] = numReps;
				done = 0;
			}
				break;
				
			case ENDREP: {
				UInt8 reps = st[--sp];
				done = 0;
				if ( --reps ) {
					i = st[sp-1];  // leave on stack
					st[sp++] = reps;
				} else {
					--sp; // remove i
				};
				}
				break;

			case END:
				i = 0;
				done = 0;
			break;

			case SUSPEND:
				suspendCallbackTimer(m);
				stepperSpeed(m,0);
			break;

			case WAIT:
				stepperSpeed(m,0);
				setCallbackTimer(m,speeds[sqp[i++]]);
			break;

			case RESTART:
				releaseCallbackTimer(sqp[i++]);
				done = 0;
				break;

			case STEP:
				stepperSpeed(m,steps[sqp[i++]]);
				setCallbackTimer(m,speeds[sqp[i++]]);
			break;
							
		};
	};
	players[m].sp = sp;
	players[m].index = i;

}



UInt8 down = 1;

const UInt16 speedChoices[] = {
	12,10,9,
};
const scSize = sizeof(speedChoices)/sizeof(UInt16);


UInt16 speed;
typedef enum _switchStates {
	CheckSwitch,
	SwitchDebounce
} SwitchStates;

typedef struct _switchState {
	UInt8			down;
	UInt16			count;
	UInt8			mask;
} SwitchState;

SwitchState	switchState[2];
#define DEBOUNCE_COUNT	5
#define MAX_SPEED		60
#define MIN_SPEED		5


void
initSwitches() {
	UInt8 i;
	for ( i = 0; i < 2; ++i ) {
		switchState[i].down = 0;
		switchState[i].count = DEBOUNCE_COUNT;
		switchState[i].mask = 0x2 << i;
	}
};

void
checkSwitches()
{
	UInt8 i;
	for ( i = 0; i < 2; ++i ) {
		SwitchState *sp = &switchState[i];
		if ( PORTA & sp->mask ) {
			sp->count = DEBOUNCE_COUNT;
			sp->down = 0;
			continue;
		}
		if ( sp->down )
			continue;
		sp->count--;
		if ( !sp->count ) {
			sp->down = 1;
			if ( i ) {
				if ( speed > MIN_SPEED )
					speed--;
			} else {
				if ( speed < MAX_SPEED )
					speed++;
			}
			stepperSpeed(1,speed);
		}
	}					
}


void
main()
{	
	
	
	init();
	initInterrupt(0,1);
	initCallbackTimers();
	initSteppers();
	initSwitches();
	initPlayers();
	pseudoSeed(41);

	ei();	// enable interrupts

	
	stepperDir(0,0);
	stepperDir(1,0);
	timerCallback(0,play);
	timerCallback(1,play);
	while ( 1 ){
		CLRWDT();
		if ( oneKhz ){
			oneKhz = 0;
			checkCallbackTimers();
			checkSteppers();
			checkSwitches();
		}	
	}
}
