
#include "serial.h"
#include "midi.h"

bank1 UInt8	mbyte1;
bank1 UInt8	mbyte2;
bank1 UInt8	mchannel;
bank1 UInt8	msystem;
bank1 UInt8	mstatus;
bank1 UInt8 msong;
bank1 UInt8	msptr1;
bank1 UInt8	msptr2;
bank1 UInt8	mtimec1;
bank1 UInt8 mtimec2;
bank1 UInt8	mtest;

#define CMDBIT	0x80
#define SYSREALBIT	0x8

bit	inSystemCommon;
bit	byte1;
bit	scByte1;

#define SYSMASK				0xf0
#define	REALMASK			0x07
#define CHANNELMASK			0x0f
#define CHANNELVOICEMASK	0x07
#define	SYSTEMCOMMONMASK	0x07

#define MIDITIMECODE	0x1
#define SONGPOSITIONPTR	0x2
#define	SONGSELECT		0x3

enum RealTimeCmds {
	RealTimeClock = 0,
	RealTimeUndef, 
	RealTimeStart,
	RealTimeContinue,
	RealTimeStop,
	RealTimeUndef2,
	RealTimeActiveSensing,
	RealTimeReset
};

enum SystemCommonCmds {
	SystemExclusive = 0,
	MidiTimeCode,
	SongPosition,
	SongSelect,
	SystemCommonUndef1,
	SystemCommonUndef2,
	TuneRequest,
	EOX,
};

enum ChannelMsg {
	NoteOff = 0,
	NoteOn,
	PolyKeyPressure,
	ControlChange,
	ProgramChange,
	ChannelPressure,
	PitchWheel
};
	

#define HANDLE(task) \
	case	task : \
		handle##task (); \
		break 

#define DOHANDLE(task) \
	case	task : \
		dohandle##task (); \
		break 

#define HANDLEUNDEF(undef) \
	case	undef :	\
		handleUndef();	\
		break	


void
initmparse()
{
	mbyte1 = 0;
	mbyte2 = 0;
	inSystemCommon = 0;
	byte1 = 0;
	scByte1 = 0;
	mstatus = 0;
	handlerInit();
}

void
checkmparse(void)
{
	
	while( readChar(&mtest) )
	{
		if ( mtest & CMDBIT )	
		{
#if 1
			UInt8 tmp = mtest & SYSMASK;
			if ( tmp == SYSMASK )
#else
			if ( SYSMASK == (mtest & SYSMASK))
#endif
			{
				if ( mtest & SYSREALBIT )
				{
					switch( mtest & REALMASK )
					{
						HANDLE(RealTimeClock);
						HANDLEUNDEF(RealTimeUndef); 
						HANDLE(RealTimeStart);
						HANDLE(RealTimeContinue);
						HANDLE(RealTimeStop);
						HANDLEUNDEF(RealTimeUndef2);
						HANDLE(RealTimeActiveSensing);
						HANDLE(RealTimeReset);
					}
				}
				else
				{
					msystem = mtest & SYSTEMCOMMONMASK;
					switch( msystem )
					{
						case SystemExclusive:
						case MidiTimeCode:
						case SongPosition:
						case SongSelect:
							scByte1 = 1;	
							inSystemCommon = 1;	
							break;
						HANDLEUNDEF(SystemCommonUndef1);
						HANDLEUNDEF(SystemCommonUndef2);
						HANDLE(TuneRequest);
						DOHANDLE(EOX);
					}
				}
			}
			else
			{
				inSystemCommon = 0;
				mstatus = (mtest >> 4) & CHANNELVOICEMASK; 
				mchannel = mtest & CHANNELMASK;
				byte1 = 1;
			}
		}
		else
		{
			if ( inSystemCommon )
			{
				if ( scByte1 )
				{
					switch( msystem )
					{
						HANDLE(SystemExclusive);
						case MidiTimeCode:
							scByte1 = 0;
							mtimec1 = mtest;
							break;
						case SongPosition:
							scByte1 = 0;
							msptr1 = mtest;
							break;

						case SongSelect:
							inSystemCommon = 0;
							msong = mtest;
							handleSongSelect();
							break;
							
						HANDLEUNDEF(SystemCommonUndef1);
						HANDLEUNDEF(SystemCommonUndef2);
						HANDLEUNDEF(TuneRequest);
						HANDLEUNDEF(EOX);
					}
				}
				else
				{
		
					inSystemCommon = 0;
					switch( msystem )
					{
						HANDLEUNDEF(SystemExclusive);
						case MidiTimeCode:
							mtimec2 = mtest;
							handleMidiTimeCode();
							break;
						case SongPosition:
							msptr2 = mtest;
							handleSongPosition();
							break;

						HANDLEUNDEF(SongSelect);	
						HANDLEUNDEF(SystemCommonUndef1);
						HANDLEUNDEF(SystemCommonUndef2);
						HANDLEUNDEF(TuneRequest);
						HANDLEUNDEF(EOX);
					}
				}
			}
			else
			{
				if ( byte1 )
				{
					mbyte1 = mtest;
					switch ( mstatus )
					{
						default:
							byte1 = 0;
							break;

						HANDLE(ProgramChange);
						HANDLE(ChannelPressure);
					}
				}
				else
				{
					mbyte2 = mtest;
					byte1 = 1;
					switch ( mstatus )
					{
						HANDLE(NoteOff);
						HANDLE(NoteOn);
						HANDLE(PolyKeyPressure);
						HANDLE(ControlChange);
						HANDLE(PitchWheel);
						HANDLEUNDEF(ProgramChange);
						HANDLEUNDEF(ChannelPressure);
					}
				}
			}
		}
	}
}


