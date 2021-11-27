#include <pic.h>
#include <sys.h>
#include "interrupt.h"
#include "timers.h"
#include "random.h"
#include "sumosup.h"
#include "a2d.h"

DECLARE_A2D_VALS(7);

DECLARE_INT_FLAGS;
DECLARE_TIMERS(MaxTimers);
TWEEK_MILLI(10,18);










main()
{
	int state = 0;
	initSumo();
	initTimers();
	initInterrupt(0,1);
	initRandom();
	initA2D();
	ei();

	
	while ( 1 )
	{
		checkA2D();
		if ( oneKhz )
		{
			checkTimers();
			checkSpeed();
			oneKhz = 0;
			RD3 = RD3 ? 0 : 1;

			if ( edgeDetected() )
				moveFromEdge();

			if ( objectDetected() )
				avoidObject();
		} // if ( oneKhz )
	} // while(1)
} // main
