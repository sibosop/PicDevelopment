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

#if defined(_16C73B) || defined(_16C71)
void
initA2D()
{
	UInt8	i;
	A2DChan = 0;
	A2DState = SETUP;
#if defined(_16C73B) 
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
	ADRES = 0;		// clear the conversion buffer
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
			A2DVals[A2DChan] = ADRES;
			++A2DChan;
			if ( A2DChan == numA2DChannels ) A2DChan = 0;
			break;
		}
	}
}
#elif defined(_16F876)
void
initA2D()
{
	UInt8	i;
	A2DChan = 0;
	A2DState = SETUP;
	if ( numA2DChannels == 3 )
	{
		ADCON1 = 4;		// NOTE channels 0,1 and 3 (NOT 2!, 2 will be digital)
	}
	else if ( numA2DChannels == 1 )
	{
		ADCON1 = 0xE; 
	}
	else
	{
		ADCON1 = 0;		// This will make all channels Analog
	}

#ifdef RIGHT_JUSTIFY
	ADCON1 |= 0x80;
#endif

	ADCON0 = 0xC1;	// RC clock, anable AD, selects chan 0
	ADRESL = 0;
	ADRESH = 0;		// clear the conversion buffer
	for ( i = 0; i < numA2DChannels; ++i )
	{
		A2DVals[i] = 0;
	}
}

void
checkA2D()
{
#ifdef RIGHT_JUSTIFY
	UInt16 tmp;
#endif

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
			if ( numA2DChannels == 3 )
			{
				if ( A2DChan == 2 )
				{
					ADCON0 = (3 << 3) | 0xC1;		// if three a-d channels then channel 2 is actually read from 3
				}
				else
					ADCON0 = (A2DChan << 3) | 0xC1;
			}
			else
			{
					ADCON0 = (A2DChan << 3) | 0xC1;
			}
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

#ifdef RIGHT_JUSTIFY
			tmp = ADRESL | (ADRESH << 8);
			tmp >>= 2;
			A2DVals[A2DChan] = (UInt8)tmp;
#else
			A2DVals[A2DChan] = ADRESH;
#endif
			++A2DChan;
			if ( A2DChan == numA2DChannels ) A2DChan = 0;
			break;
		}
	}
}
#else
#error unknown device
#endif

UInt8
getA2D(UInt8 chan)
{
	return A2DVals[chan];
}
