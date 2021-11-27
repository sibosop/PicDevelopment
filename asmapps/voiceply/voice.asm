;
; This is the handler stub file for midi commands
; replace any commands handled with real code
;
	list      P=PIC16c71             ; list directive to define processor
	#include	../shared/macros.inc
	#include <p16c71.inc>         ; processor specific variable definitions
	#define		IN_VOICE
	#include	voiceply.inc
	#include 	timers.inc
	#include	a2d.inc
	#include 	math.inc
	list	


USRFILES       	UDATA   
chipState res 1			; state machine for voice chip
msgNum  res 1			; current message number
curMsg 	res 1			; temporary holder for message number tests
maxMsg 	res 1
vocTmp1	res 1
vocTmp2 res 1
loopCnt res 1


	; chip states
	constant Running   = 1
	constant MsgLookup = 3
	constant PudReset  = 4
	constant CountMsg  = 5
	constant InitFlag  = 6
	constant RandomMsg = 7


	; port B bits
	constant EomLow	           = 0	; input
	constant PowerDownHigh	   = 1	; output
	constant ChipEnableLow     = 2	; output
	constant Addr0		   = 3	; output

	constant Kilohertz  	  = 4	; input
	constant PowerDownHighLed = 2	; output
;	constant ChipEnableLowLed = 6	; output
;	constant Addr0Led	  = 7	; output


	; other constants
	constant PudWaitTime	 = D'28' ; worst case time for power reset
	constant FirstEomWaitTime = D'200'
	constant EomWaitTime	= D'100'

PROG	CODE
initVoc
	global initVoc
	banksel chipState
	clrf	chipState	
	bsf	chipState,InitFlag
	bsf	chipState,Running
	clrf	msgNum
	clrf	curMsg
	clrf	maxMsg
	clrf	loopCnt
	bsf	loopCnt,0
	banksel	PORTB
	bsf	PORTB,ChipEnableLow
;	bsf	PORTB,ChipEnableLowLed
	bcf	PORTB,Addr0	
;	bcf	PORTB,Addr0Led	
	bsf	PORTB,PowerDownHigh
	bsf	PORTA,PowerDownHighLed
	return




	

checkPud
	global	checkPud
	banksel	chipState
	btfsc	chipState,InitFlag
	return
	btfsc	timermask,pudwaittimer	; has the power up timer expired?
	return				; no then return
	banksel	state
	bcf	state,PudWait		; yes, clear the state
	banksel	chipState
	btfsc	chipState,PudReset	; are we in a reset
	goto	finishReset		; yes, then finish it
	goto	playNext		; no, then play

checkEom
	global	checkEom
	banksel chipState
	btfsc	chipState,InitFlag
	return
	btfsc	chipState,MsgLookup	; are we in msg lookup
	goto	playNext		; yes, then test current msg
doPudReset
	banksel	PORTB			; no, we need to reset the addr
	bsf	PORTB,PowerDownHigh	; by bouncing the power 
	bsf	PORTA,PowerDownHighLed	; by bouncing the power 
	banksel	chipState
	bsf	chipState,PudReset	; say we are in power reset
	clrf	curMsg			; reset, resets current msg
	bsf	chipState,MsgLookup	; if in play we need to lookup msg
	goto	doPudWait		; set up a timer wait
	
finishReset
	bcf	chipState,PudReset	; clear the reset flag
	btfss	chipState,Running	; are we running
	return				; nope
	banksel	PORTB
	bcf	PORTB,PowerDownHigh	; power up the chip and wait
	bcf	PORTA,PowerDownHighLed	; power up the chip and wait
doPudWait
	banksel	timermask
	stimw	pudwaittimer,PudWaitTime; set the power up timer 
	banksel	state			; wait time is worse case
	bsf	state,PudWait		; set the state to power up wait
	return

playNext
	btfsc	chipState,CountMsg
	goto	countMsg
	movf	curMsg,w		; get the current msg index
	subwf	msgNum,w		; test to see if current = requested
	btfss	STATUS,Z		; zero means yes
	goto	bumpMsg			; no, then bump current msg
	call	chooseRandomMsg		; yes, then set msg for next time
	bcf	chipState,MsgLookup	; tell eom we are not in msg lookup
	banksel	PORTB			
	bcf	PORTB,Addr0		; set normal play mode
;	bcf	PORTB,Addr0Led		; set normal play mode
pulseEnable
	bcf	PORTB,ChipEnableLow	; pulse to start msg
	bsf	PORTB,ChipEnableLow
;	bsf	PORTB,ChipEnableLowLed
	return
bumpMsg
	banksel	curMsg
	incf	curMsg,f		; bump the msg
doSearch
	banksel	PORTB			
	bsf	PORTB,Addr0		; set to fast lookup mode
;	bsf	PORTB,Addr0Led		; set to fast lookup mode
	goto	pulseEnable

countMsg
	stimw	eomTimer,EomWaitTime
	incf	maxMsg,f
	banksel	PORTB			
	bsf	PORTB,Addr0		; set to fast lookup mode
;	bsf	PORTB,Addr0Led		; set to fast lookup mode
	goto	pulseEnable
	

checkVoc
	global	checkVoc
	banksel	chipState
	btfsc	chipState,InitFlag	; should we init
	goto	doInit			; yes
	btfss	chipState,CountMsg	; no, are we counting messages?
	goto	testRunning			; no
	btfsc	timermask,eomTimer	; yes have we timed out counting msgs
	return				; no
	banksel chipState
	bcf	chipState,CountMsg	; clear the count message flag
	decf	maxMsg,w		; dec the max if it's not zero
	andlw	0x7f			; 127 is the largest allowed message	
	movwf	maxMsg			; save legal msg size
	decf	maxMsg,f		; dec the max if it's not zero
	call	setRandom		;
	goto	doStop			; stop the msg counting
testRunning
	call	checkctlr
	return


	
doInit
	clrf	maxMsg
	bcf	chipState,InitFlag	; only set once
	bsf	chipState,CountMsg	; set up for msg counting
	stimw	eomTimer,FirstEomWaitTime
doStop
	banksel	PORTB			
	bcf	PORTB,Addr0		; normal playback
;	bcf	PORTB,Addr0Led		; normal playback
	bsf	PORTB,ChipEnableLow	; no record
;	bsf	PORTB,ChipEnableLowLed	; no record
	bsf	PORTB,PowerDownHigh	; no power
	bsf	PORTA,PowerDownHighLed	; no power
startUp
	banksel curMsg
	clrf	curMsg			; clear the message index
	goto	doPudReset		; powerup

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

stopVoice
	global	stopVoice
	bcf	chipState,Running
	return

startVoice
	global	startVoice
	btfsc	chipState,Running
	return
	bsf	chipState,Running
	goto	startUp

	END

