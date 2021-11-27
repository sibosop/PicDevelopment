


; define chip to be used and include the standard stuff
	processor	16C73B
	#define	IN_MPARSE
	#include	P16C73B.INC
	#define		UARTSUPP
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	minput.inc
	#include	mparse.inc
	#include	queue.inc
	#include	uart.inc
	
;
; these handlers are implemented in mhandler.asm
; The midi parser found in this file should not have
; to be touched.
;
	extern handleNoteOff
	extern handleNoteOn
	extern handlePolyphonicKeyPressure
	extern handleProgramChange
	extern handleControlChange
	extern handleChannelPressure
	extern handlePitchWheel
	extern handleTuneRequest
	extern handleTimeCode
	extern handleSystemExclusive
	extern handleEOX
	extern handleTimeClock
	extern handleUndefined
	extern handleStart
	extern handleContinue
	extern handleStop
	extern handleUndefined
	extern handleActiveSensing
	extern handleReset
	extern handleTuneRequest
	extern handleSongSelect
	extern handleSongPositionPtr

; define local registers, etal

RAM2	UDATA	
mtest  	res	1		; midi input byte
mbyte1	res	1		; storage for param byte 1
mbyte2	res	1		; storage for param byte 2
mstatus	res	1		; current running status
mchannel res	1		; channel
msystem res	1		; current system byte
mstate	res	1		; needed states between bytes
msong	res	1		; song number
msptr1	res	1		; song pointer byte one
msptr2	res	1		; song poinger byte two
mtimec1	res	1		; midi time code byte one
mtimec2	res	1		; midi time code byte two
	extern	tmp4

	global	mbyte1		; for the handlers
	global	mbyte2		; for the handlers
	global	msong		; for the handlers
	global	msptr1		; for the handlers
	global	msptr2		; for the handlers
	global	mtimec1		; for the handlers
	global	mtimec2		; for the handlers
	global	mchannel	; for the handlers

	constant cmdbit   = 7	; test bit to determine cmd/value
	constant sysreal  = 3	; test bit to determine real time cmd

; status flags
	constant InSystemCommon = 0; status saying we are in a sytem parse
	constant Byte1 = 1	; status saying we are waiting for byte 1
	constant SCByte1 = 2	; status saying we are waiting for system byte 1
	constant debug	= 3

	constant SysMask  = 0xf0	; mask for all system calls
	constant RealMask = 0x7		; mask for obtaining realtime cmds
	constant ChannelMask = 0xf	; mask for getting the channel
	constant ChannelVoiceMask = 0x7	; mask for getting channel voice cmd
	constant SystemCommonMask = 0x7	; mask for getting system common cmd

	; needed by the system common logic
	constant MidiTimeCode	= 0x1	; MIDI command for time code
	constant SongPositionPtr = 0x2 	; MIDI command for Song Position
	constant SongSelect = 0x3	; MIDI command for Song Select

; Put it in ROM PAGE0
PROG1	CODE
mdebug	macro	port,bit
	btfss	mstate,debug
	goto	ledon
	banksel	port
	bcf	port,bit
	banksel	mstate
	bcf	mstate,debug
	goto	db1
ledon
	banksel port
	bsf	port,bit
	banksel mstate
	bsf	mstate,debug
db1
	endm

;
; initialize the midi subsystem
;
mparseinit
	global mparseinit
	clrf	mbyte1
	clrf	mbyte2
	clrf	mstatus
	clrf	msystem
	clrf	mstate
	extern	handlerInit
	call	handlerInit
	return

checkmparse
	global	checkmparse
rdlp
	clrf	PCLATH			; all this code is below 0x100
	disable
	dequeue	readbuf			; get the next midi byte
	enable
	btfsc	STATUS,Z		; zero means we're done
	return				; we're done
	banksel	mtest
	movwf	mtest			; save the midi byte for testing
	btfss	mtest,cmdbit		; is it a command byte?
	goto	doValue			; no then do the value
	movlw 	SysMask			; get the system test mask
	andwf	mtest,w			; mask of the system bytes
	sublw	SysMask			; compare to the system mask
	btfss	STATUS,Z		; zero means it's a system cmd
	goto	doChannel		; not zero, do the channel cmd
	btfss	mtest,sysreal		; see if it's a real time message
	goto	doSystemCommon		; no? then it's system common
	movlw	RealMask		; get the mask for realtime messages
	andwf	mtest,w			; mask off the command values
	goto	doRealtime		; we're doing a realtime command
	goto	rdlp

doValue
	btfsc	mstate,InSystemCommon	; are we looking for system params
	goto	doSystemCommonByte	; yes, then use byte for that
	btfss	mstate,Byte1		; are we waiting for the first byte
	goto	doByte2			; no, do the second byte
	movf	mtest,w			; get the input byte
	movwf	mbyte1			; store it as byte one
	movf	mstatus,w		; get the running status
	addwf	PCL,f			; jmp accordingly
	goto	setByte2		; note off needs 2 bytes
	goto	setByte2		; note on needs 2 bytes
	goto	setByte2		; poly key needs 2 bytesa
	goto	setByte2		; control change needs 2 bytes
	goto	doHandleProgramChange	; do program change since we have one
	goto	doHandleChannelPressure	; do channel pressure
setByte2				; Pitch wheel needs 2 bytes
	bcf	mstate,Byte1		; say that we have byte 1
	goto	rdlp			; look for next byte
doByte2	
	movf	mtest,w			; get the input byte
	movwf	mbyte2			; store it as byte2
	bsf	mstate,Byte1		; say we are looking for byte1 again
;	movlw	high doHandleNoteOff
;	movwf	PCLATH
	movf	mstatus,w		; get the running status byte
	addwf	PCL,f			; jmp accordingly
	goto	doHandleNoteOff	
	goto	doHandleNoteOn
	goto	doHandlePolyphonicKeyPressure
	goto	doHandleControlChange
	goto	doHandleProgramChange	; shouldn't get here, but hey
	goto	doHandleChannelPressure	; shouldn't get here, but hey
	goto	doHandlePitchWheel

	
	
doChannel
	bcf	mstate,InSystemCommon	; we're not in a system cmd
	swapf	mtest,w			; get channel msg type into low nib
	andlw	ChannelVoiceMask	; mask off the channel msg
	movwf	mstatus			; save for running status
	movlw	ChannelMask		; get the channel maksk 
	andwf	mtest,w			; mask off the channel number
	movwf	mchannel		; save the channel number
	bsf	mstate,Byte1		; set that the next byte is byte one
	goto	rdlp			; get next byte

doSystemCommon
;	movlw	high setInSystemCommon
;	movwf	PCLATH
	movlw	SystemCommonMask	; get the mask for system common
	andwf	mtest,w			; mask out the bites in the input
	movwf	msystem			; save the system cmd
	addwf	PCL,f			; jmp accordingly
	goto	setInSystemCommon	; system exclusive
	goto	setInSystemCommon	; midi time code
	goto	setInSystemCommon	; song position
	goto	setInSystemCommon	; song select
	goto	doHandleUndefined
	goto	doHandleUndefined
	goto	doHandleTuneRequest
	goto	doHandleEOX

setInSystemCommon
	bsf	mstate,SCByte1		; we are waiting for a system byte
	bsf	mstate,InSystemCommon		; we are in system command
	goto	rdlp			; get the next byte

doSystemCommonByte
	btfss	mstate,SCByte1		; ready for the first byte?
	goto	doSByte2		; this is the second byte
;	movlw	high doHandleSystemExclusive
;	movwf	PCLATH
	movf	msystem,w
	addwf	PCL,f
	goto	doHandleSystemExclusive
	goto	saveTimeCode1
	goto	saveSongPositionPtr1
	goto	doHandleSongSelect
	goto	doHandleUndefined
	goto	doHandleUndefined
	goto	doHandleTuneRequest
	goto	doHandleEOX
	
saveTimeCode1
	bcf	mstate,SCByte1
	movf	mtest,w
	movwf	mtimec1
	goto	rdlp

saveSongPositionPtr1
	bcf	mstate,SCByte1
	movf	mtest,w
	movwf	msptr1
	goto	rdlp
	

doSByte2
;	movlw	high doHandleSystemExclusive
;	movwf	PCLATH
	movf	msystem,w
	addwf	PCL,f
	goto	doHandleSystemExclusive	; shouldn't happen
	goto	doHandleTimeCode
	goto	doHandleSongPositionPtr
	goto	doHandleSongSelect
	goto	doHandleUndefined
	goto	doHandleUndefined
	goto	doHandleTuneRequest
	goto	doHandleEOX

doRealtime
	addwf	PCL,f
	goto	doHandleTimeClock
	goto	doHandleUndefined
	goto	doHandleStart
	goto	doHandleContinue
	goto	doHandleStop
	goto	doHandleUndefined
	goto	doHandleActiveSensing
	goto	doHandleReset

doHandleNoteOff
	call handleNoteOff
	goto rdlp

doHandleNoteOn
	call handleNoteOn
	goto rdlp

doHandlePolyphonicKeyPressure
	call handlePolyphonicKeyPressure
	goto rdlp

doHandleControlChange
	call handleControlChange
	goto rdlp

doHandleProgramChange
	call handleProgramChange
	goto rdlp

doHandleChannelPressure
	call handleChannelPressure
	goto rdlp

doHandlePitchWheel
	call handlePitchWheel
	goto rdlp

doHandleTuneRequest
	call handleTuneRequest
	goto rdlp

doHandleEOX
	bcf  mstate,InSystemCommon
	call handleEOX
	goto rdlp

doHandleSystemExclusive
  	bsf  mstate,SCByte1		; exclusive always uses byte 1
	movf mtest,w
	call handleSystemExclusive
	goto rdlp

doHandleTimeCode
	movf mtest,w
	movwf mtimec2
	bcf  mstate,InSystemCommon
	call handleTimeCode
	goto rdlp

doHandleTimeClock
	call handleTimeClock
	goto rdlp

doHandleUndefined
	bcf  mstate,InSystemCommon
	call handleUndefined
	goto rdlp

doHandleStart
	call handleStart
	goto rdlp

doHandleContinue
	call handleContinue
	goto rdlp

doHandleStop
	call handleStop
	goto rdlp

doHandleActiveSensing
	call handleActiveSensing
	goto rdlp

doHandleReset
	call handleReset
	goto rdlp

doHandleSongSelect
	bcf  mstate,InSystemCommon
	movf mtest,w
	movwf msong
	call handleSongSelect
	goto rdlp

doHandleSongPositionPtr
	bcf  mstate,InSystemCommon
	movf mtest,w
	movwf msptr2
	call handleSongPositionPtr
	goto rdlp


	END
