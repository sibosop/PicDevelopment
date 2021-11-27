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
		if (useA2DRef)
			ADCON1 = 1;		// sets up channel 3 as a ref channel
		else
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
					if ( useA2DRef && A2DChan == 3 )
					{
						++A2DChan;	// skip the ref channel
					}
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
#error	RIGHT_JUSTIFY
			//tmp = ADRESL | (ADRESH << 8);
			//tmp >>= 2;
			tmp = ADRESH;
			tmp <<= 8;
			tmp |= ADRESL;
			A2DVals[A2DChan] = tmp;
#else
			A2DVals[A2DChan] = ADRESH;
#endif
			++A2DChan;
			if ( A2DChan == numA2DChannels ) A2DChan = 0;
			break;
		}
	}
} 
#elif defined(_16F877A)
void
initA2D()
{
	UInt8	i;
	A2DChan = 0;
	A2DState = SETUP;
#ifdef USE_REFERENCE_VOLTAGE
	switch ( numA2DChannels )
	{
		default:
		case 1:
			ADCON1 = 0xf;
			break;
		case 2:
			ADCON1 = 0xd;
			break;
		case 3:
			ADCON1 = 0xc;
			break;
		case 4:
			ADCON1 = 0xb;
			break;
		case 5:
			ADCON1 = 0xa;
			break;
		case 6:
			ADCON1 = 8;
			break;
		case 7:
			ADCON1 = 1;
			break;
	}
#else
	switch ( numA2DChannels )
	{
		default:
		case 1:
			ADCON1 = 0xe;
			break;
		case 3:
			ADCON1 = 4;
			break;
		case 5:
			ADCON1 = 2;
			break;
		case 6:
			ADCON1 = 9;
			break;
		case 8:
			ADCON1 = 0;
			break;
	} 
#endif

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
#ifdef USE_REFERENCE_VOLTAGE
			switch ( numA2DChannels )
			{
				default:
				case 1:
				case 2:
					ADCON0 = (A2DChan << 3) | 0xC1;
					break;
				case 3:
				case 4:
				case 6:
					if ( A2DChan > 2 )
						ADCON0 = ((A2DChan+2)<<3) | 0xC1;
					else 
						ADCON0 = (A2DChan << 3) | 0xC1;
					break;
				case 5:
				case 7:
					if ( A2DChan > 3 )
						ADCON0 = ((A2DChan+1)<<3) | 0xC1;
					else 
						ADCON0 = (A2DChan << 3) | 0xC1;
					break;
			}
#else
			ADCON0 = (A2DChan << 3) | 0xC1;
#endif	
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
			tmp = ADRESH;
			tmp <<= 8;
			//tmp = ADRESL | (ADRESH << 8);
			tmp |= ADRESL;
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

UInt16
getA2D(UInt8 chan)
{
	return A2DVals[chan];
}
