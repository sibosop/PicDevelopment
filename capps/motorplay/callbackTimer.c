#include "callbackTimer.h"


void initCallbackTimers(void)
{
	UInt8 i;
	for( i = 0; i < numTimers; ++i ) {
		callbackTimers[i].value = 0;
		callbackTimers[i].timer = 0;
		callbackTimers[i].cb = 0;
		callbackTimers[i].suspend = 0;
	}
}

void checkCallbackTimers(void)
{
	UInt8 timer;
	for ( timer = 0; timer < numTimers; ++timer) 
	{
		CBANK CallbackTimers *cp = &callbackTimers[timer];
		if ( cp->suspend )
			continue;

		if ( cp->timer ){
			cp->timer--;
		}
		else {
			cp->timer = cp->value;
			cp->cb(timer);
		}
	}
}


void
setCallbackTimer(UInt8 timer, UInt16 time ){
	callbackTimers[timer].timer = time;
	callbackTimers[timer].value = time;
} 

void
releaseCallbackTimer(UInt8 timer) {
	callbackTimers[timer].suspend = 0;
	callbackTimers[timer].timer = 0;

}

void
timerCallback(UInt8 t, void(*cb)(UInt8)) {
	callbackTimers[t].cb = cb;
}

void
suspendCallbackTimer(UInt8 t) {
	callbackTimers[t].suspend = 1;
}
