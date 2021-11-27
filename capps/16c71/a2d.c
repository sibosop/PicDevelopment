#include <pic.h>
#include <sys.h>
#include "a2d.h"


UInt8 A2DChan;
UInt8 A2DState;

enum A2DStates{
	SETUP = 0,
	SETUP_WAIT,
	START_CONV,
	WAIT_FOR_CONV
};

void
initA2D()
{
	UInt8	i;
	A2DChan = 0;
	A2DState = SETUP;
#if defined(_16C73B) || defined (_16F876)
	if ( numA2DChannels < 4 )
	{
		ADCON1 = 4;
	}
#else
	if ( numA2DChannels < 3 )
	{
		ADCON1 = 2;
	}
#endif
	else
	{
		ADCON1 = 0;	
	}
	ADCON0 = 0xC1;	// RC clock, anable AD, selects chan 0
#if defined(_16F876)

	ADRESL = 0;
#else
	ADRES = 0;		// clear the conversion buffer
#endif

	for ( i = 0; i < numA2DChannels; ++i )
	{
		A2DVals[i] = 0;
	}
}

void
checkA2D()
{
	switch (A2DState)
	{
		default:
		{
			A2DState = SETUP;
			break;
		}

		case SETUP:
		{
			++A2DState;
			ADCON0 = (A2DChan << 3) | 0xC1;
			break;
		}

		case SETUP_WAIT: 
		{
			++A2DState;
			break;
		}

		case START_CONV: 
		{
			++A2DState;
			ADGO = 1; 
			break;
		}
		case WAIT_FOR_CONV:

		{
			if ( ADGO ) break;
			A2DState = SETUP;
#if defined(_16F876)
			A2DVals[A2DChan] = ADRESL;
#else
			A2DVals[A2DChan] = ADRES;
#endif
			++A2DChan;
			if ( A2DChan == numA2DChannels ) A2DChan = 0;
			break;
		}
	}
}

UInt8
getA2D(UInt8 chan)
{
	return A2DVals[chan];
}
