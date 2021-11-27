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
chipState res 1			; state machine for voice chip
chipState2 res 1
msgNum  res 1			; current message number
curMsg 	res 1			; temporary holder for message number tests
maxMsg 	res 1
vocTmp1	res 1
vocTmp2 res 1
loopCnt res 1

	; chip states
	constant Record = 0
	constant Paused = 1
	constant Running = 2
	constant MsgLookup = 3
	constant PudReset  = 4
	constant CountMsg  = 5
	constant InitFlag  = 6
	constant RandomMsg = 7


	; port B bits
	constant EomLow	           = 0	; input
	constant PowerDownHigh	   = 1	; output
	constant RecordEnableLow   = 2	; output
	constant ChipEnableLow     = 3	; output
	constant Addr0		   = 4	; output
	constant Overflow	   = 5	; input

	; other constants
	constant PudWaitTime	 = D'28' ; worst case time for power reset
	constant FirstEomWaitTime = D'200'
	constant EomWaitTime	= D'100'
	constant MidiRandomMsgNum  = D'127'

PROG1	CODE
initVoc
	global initVoc
	banksel chipState
	clrf	chipState	
	bsf	chipState,InitFlag
	clrf	msgNum
	clrf	curMsg
	clrf	maxMsg
	clrf	loopCnt
	bsf	loopCnt,0
	banksel	PORTB
	bsf	PORTB,RecordEnableLow
	bsf	PORTB,ChipEnableLow
	bsf	PORTA,1
	bcf	PORTB,Addr0	
	bsf	PORTB,PowerDownHigh
	bsf	PORTA,2
	bcf	PORTA,0
	return




	

checkPud
	global	checkPud
	banksel	timermask
	btfsc	timermask,pudwaittimer	; has the power up timer expired?
	return				; no then return
	banksel	state
	bcf	state,pudwait		; yes, clear the state
	banksel	chipState
	btfsc	chipState,PudReset	; are we in a reset
	goto	finishReset		; yes, then finish it
	btfss	chipState,Running	; are we still running
	return				; no
	btfss	chipState,Record	; are we in Record
	goto	playNext		; no, then play
	banksel	PORTB
	bcf	PORTB,ChipEnableLow
	bcf	PORTA,1
	return

checkEom
	global	checkEom
	banksel chipState
	btfss	chipState,Running	; are we playing already
	return				; yes
	btfsc	chipState,Paused	; are we paused
	return				; yes
	btfsc	chipState,Record
	goto	handleOverflow		; EOM= overflow in record
	btfsc	chipState,MsgLookup	; are we in msg lookup
	goto	playNext		; yes, then test current msg
doPudReset
	banksel	PORTB			; no, we need to reset the addr
	bsf	PORTB,PowerDownHigh	; by bouncing the power 
	bsf	PORTA,2
	banksel	chipState
	bsf	chipState,PudReset	; say we are in power reset
	clrf	curMsg			; reset, resets current msg
	bsf	chipState,MsgLookup	; if in play we need to lookup msg
	goto	doPudWait		; set up a timer wait
	
finishReset
	bcf	chipState,PudReset	; clear the reset flag
	btfss	chipState,Record
	goto	skipRec
	banksel	PORTB
	bcf	PORTB,RecordEnableLow
	bcf	PORTB,Addr0
skipRec
	banksel	PORTB
	bcf	PORTB,PowerDownHigh	; power up the chip and wait
	bcf	PORTA,2
doPudWait
	banksel	timermask
	stimw	pudwaittimer,PudWaitTime; set the power up timer 
	banksel	state			; wait time is worse case
	bsf	state,pudwait		; set the state to power up wait
	return

playNext
	btfsc	chipState,CountMsg
	goto	countMsg
	movf	curMsg,w		; get the current msg index
	subwf	msgNum,w		; test to see if current = requested
	btfss	STATUS,Z		; zero means yes
	goto	bumpMsg			; no, then bump current msg
	btfsc	chipState,RandomMsg	; are we in random msg mode
	call	chooseRandomMsg		; yes, then set msg for next time
	bcf	chipState,MsgLookup	; tell eom we are not in msg lookup
	banksel	PORTB			
	bcf	PORTB,Addr0		; set normal play mode
pulseEnable
	bcf	PORTB,ChipEnableLow	; pulse to start msg
	bsf	PORTB,ChipEnableLow
	bsf	PORTA,1
	return
bumpMsg
	banksel	curMsg
	incf	curMsg,f		; bump the msg
doSearch
	banksel	PORTB			
	bsf	PORTB,Addr0		; set to fast lookup mode
	goto	pulseEnable

countMsg
	stimw	eomTimer,EomWaitTime
	incf	maxMsg,f
	banksel	PORTB			
	bsf	PORTB,Addr0		; set to fast lookup mode
	goto	pulseEnable
	
	

handleOverflow
	global	handleOverflow
	banksel	chipState
	bcf	chipState,Record
	bcf	chipState,Paused
	bcf	chipState,Running
	goto	doStop

checkVoc
	global	checkVoc
	banksel	chipState
	btfsc	chipState,InitFlag	; should we init
	goto	doInit			; yes
	btfss	chipState,CountMsg	; no, are we counting messages?
	return				; no
	btfsc	timermask,eomTimer	; yes have we timed out counting msgs
	return				; no
	banksel	PORTA			; yes
	bsf	PORTA,0			; flag done counting
	banksel chipState
	bcf	chipState,CountMsg	; clear the count message flag
	movf	maxMsg,f		; we need to dec the count by one
	btfsc	STATUS,Z		; sanity check that it's not zero
	goto	doStop			; stop counting
	decf	maxMsg,f		; dec the max if it's not zero
	goto	doStop			; stop the msg counting
	
doInit
	clrf	maxMsg
	bcf	chipState,InitFlag	; only set once
	bsf	chipState,CountMsg	; set up for msg counting
	stimw	eomTimer,FirstEomWaitTime
	goto	doPlay

	
	


;
; control message controls the voice chip
; 0 - play
; 1 - record
; 2 - stop
; 3 - pause
; 4 - resume
;
handleControlChange
	global handleControlChange
	movf	mbyte1,w		; get the control number
	sublw	4			; is it 4?
	btfss	STATUS,Z
	return				; no, then return
	movf	mbyte2,f
	btfsc	STATUS,Z
	goto	doPlay			; = 0
	decf	mbyte2,f
	btfsc	STATUS,Z
	goto	doRecord		; = 1
	decf	mbyte2,f		
	btfsc	STATUS,Z
	goto	doStop			; = 2
	decf	mbyte2,f		
	btfsc	STATUS,Z
	goto	doPause			; = 3
	decf	mbyte2,f		
	btfsc	STATUS,Z
	goto	doResume		; = 4
	return

doPlay
	btfsc	chipState,Running	; are we playing already
	return				; yes
	btfsc	chipState,Paused	; are we paused
	return				; yes
	bsf	chipState,Running	; say we are running
	bcf	chipState,Record	; say we are in playback
	clrf	curMsg			; clear the message index
	goto	doPudReset		; powerup

doRecord
	btfsc	chipState,Running
	return
	btfsc	chipState,Paused
	return
	bsf	chipState,Running
	bsf	chipState,Record
	goto	doPudReset

doStop
	banksel	chipState
	bcf	chipState,Running
	bcf	chipState,Paused	; clear a pause
	btfss	chipState,Record
	goto	stopPlayback
	bcf	chipState,Record
	bsf	chipState,InitFlag	; set init to recount messages
	banksel	PORTA
	bcf	PORTA,0
stopPlayback
	banksel	PORTB			
	bcf	PORTB,Addr0		; normal playback
	bsf	PORTB,ChipEnableLow	; no record
	bsf	PORTA,1
	bsf	PORTB,RecordEnableLow	; playback mode
	bsf	PORTB,PowerDownHigh	; no power
	bsf	PORTA,2
	return

doPause
	banksel	chipState
	btfss	chipState,Running	; are we running
	return				; no
	btfsc	chipState,Paused	; are we already paused
	return				; yes
	bsf	chipState,Paused	; say we are paused
	btfss	chipState,Record	; are we in record
	return				; nothing else we need to do for play
	banksel	PORTB				
	bsf	PORTB,ChipEnableLow	; stop recording
	bsf	PORTA,1
	return

doResume
	banksel chipState
	btfss	chipState,Running	; are we running
	return				; no
	btfss	chipState,Paused	; are we paused
	return				; no
	bcf	chipState,Paused	; clear pause
	btfss	chipState,Record	; are we in play?
	goto	doPlay			; continue play
	banksel	PORTB
	bcf	PORTB,ChipEnableLow	; record next message
	bcf	PORTA,1
	return
	

handleProgramChange
	global 	handleProgramChange
	banksel	PORTA
	bcf	PORTA,0
	banksel	mbyte1
	bcf	chipState,RandomMsg
	movf	mbyte1,w		; get the msg number
	movwf	msgNum			; set for next playback
	sublw	MidiRandomMsgNum
	btfsc	STATUS,Z
	goto	setRandom
	movf	mbyte1,w
	subwf	maxMsg,w		; is it bigger than the max
	btfsc	STATUS,Z		; 
	goto	msgErr			; it's equal the max, just as bad
	btfsc	STATUS,C		; is it bigger
	return				; no, then were okay
msgErr
	banksel	PORTA			; yes
	bsf	PORTA,0			; flag it
	banksel	msgNum			; 
	clrf	msgNum			; clear msg to aviod chip hang
	return

setRandom
	bsf	chipState,RandomMsg	
	clrf	loopCnt
	bsf	loopCnt,0
	goto	setupNextMsg

chooseRandomMsg
	decf	loopCnt,f
	btfss	STATUS,Z
	return
setupNextMsg
	randnum D'1',D'4',1
	movwf	loopCnt
	clrf	vocTmp1			; set up the next random message
	movf	maxMsg,w
	movwf	vocTmp2
	randnum	vocTmp1,vocTmp2,0
	movwf	msgNum
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

handleNoteOff
	global handleNoteOff
	return

handleNoteOn
	global handleNoteOn
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
