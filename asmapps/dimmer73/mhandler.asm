;
; This is the handler stub file for midi commands
; replace any commands handled with real code
;
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#include 	minput.inc
	#include 	math.inc
	list	

	extern 	mbyte1	; status byte 1
	extern	mbyte2	; status byte 2
	extern	mchannel ; channel number
	extern	msong	; song number
	extern	msptr1	; song pointer byte one
	extern	msptr2	; song poinger byte two
	extern	mtimec1	; midi time code byte one
	extern	mtimec2	; midi time code byte two
RAM2	UDATA


PROG1	CODE

handleNoteOff
	global handleNoteOff
	return

handleNoteOn
	global handleNoteOn
	return
	

handleSystemExclusive
	global handleSystemExclusive
	return

handleEOX
	global handleEOX
	return

handlePolyphonicKeyPressure
	global handlePolyphonicKeyPressure
	return

handleControlChange
	global handleControlChange
	return

handleProgramChange
	global handleProgramChange
	return

handleChannelPressure
	global handleChannelPressure
	return

handlePitchWheel
	global handlePitchWheel
	return

handleTimeCode
	global handleTimeCode
	return

handleTuneRequest
	global handleTuneRequest
	return


handlerInit
	global	handlerInit
	return


handleTimeClock
	global handleTimeClock
	return



handleUndefined
	global handleUndefined
	return

handleStart
	global handleStart
	return

handleContinue
	global handleContinue
	return

handleStop
	global handleStop
	return

handleActiveSensing
	global handleActiveSensing
	return

handleReset
	global handleReset
	return

handleSongSelect
	global handleSongSelect
	return

handleSongPositionPtr
	global handleSongPositionPtr
	return

	END
