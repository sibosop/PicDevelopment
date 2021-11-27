;
; This is the handler stub file for midi commands
; replace any commands handled with real code
;
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#define		IN_VOICE
	#include	voicerec.inc
	#include 	timers.inc
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
chipState res 1


PROG1	CODE
	constant PowerDownHigh	   = 0
	constant RecordEnableLow   = 1
	constant ChipEnableLow     = 2

	constant playback = 0
initVoc
	global initVoc
	banksel	PORTA
	bsf	PORTA,PowerDownHigh
	bsf	PORTA,ChipEnableLow
	bsf	PORTA,RecordEnableLow
	banksel chipState
	clrf	chipState	
	return

handleNoteOff
	global handleNoteOff
	return

handleNoteOn
	global handleNoteOn
	banksel	mbyte2
	btfss	chipState,playback
	goto	doRecord
	movf	mbyte2,f
	btfss	STATUS,Z
	return
	movf	mbyte1,w
	banksel	PORTA
	movwf	PORTB
	bsf	PORTA,RecordEnableLow
	bcf	PORTA,PowerDownHigh
	bcf	PORTA,ChipEnableLow
	bsf	PORTA,ChipEnableLow
	return

stopPlayback
	global	stopPlayback
	banksel	PORTA
	bsf	PORTA,PowerDownHigh
	return
	
doRecord
	movf	mbyte2,f
	btfsc	STATUS,Z
	goto	stopRecord
	banksel	mbyte1
	movf	mbyte1,w
	banksel	PORTB
	bcf	PORTA,RecordEnableLow
	movwf	PORTB
	bcf	PORTA,PowerDownHigh
	stimw	pudwaittimer,D'26'
	banksel	state
	bsf	state,pudwait
	return


checkPud
	global	checkPud
	banksel	timermask
	btfsc	timermask,pudwaittimer
	return
	banksel	state
	bcf	state,pudwait
	banksel	PORTA
	bcf	PORTA,ChipEnableLow
	banksel mbyte2
	return

stopRecord
	banksel PORTA
	bsf	PORTA,ChipEnableLow
	bsf	PORTA,RecordEnableLow
	bsf	PORTA,PowerDownHigh
	banksel mbyte2
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
