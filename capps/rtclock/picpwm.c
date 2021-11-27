#include <pic.h>
#include "types.h"
#include "picpwm.h"


void
initPwm() {
	T2CON = 2; // prescale 16
	PR2 = 0xFF;	// 1.2Khz
	TMR2ON = 1;
	CCP1CON = 0xC;
	CCP2CON = 0xC;
}

