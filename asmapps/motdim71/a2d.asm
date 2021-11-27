
	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#include	"..\shared\macros.inc"
	#include	math.inc
	#include	timers.inc
	#include	motodim.inc

	LIST

	constant NumChans	= 2

USRFILES	UDATA
a2dvals	 res	NumChans	; a to d vals for each chnnel
a2dflags res	1		; flags say whether the value has changed
a2dchan	 res	1		; current a to d channeld
a2dstate res	1		; a to d state machine
cmask	res 1
cnum	res 1
	constant	ControlBase = 4
	
	global	a2dvals
	global	a2dflags

	extern	tmp1		; needed for address conversions

PROG	CODE

a2dinit
	global	a2dinit
	banksel	a2dflags	
	clrf	a2dflags	; clear the 'changed flags'
	clrf	a2dchan 	; clear the current a to d channel
	clrf	a2dstate	; put the state machin in setup mode
	banksel	TRISA		; set A to A0-1 inputs, A2-A3 out A4 in
	movlw	0x13		; set A to A0-1 inputs, A2-A3 out A4 in
	movwf	TRISA		; 
; *** this is a change from the midi implementation
	banksel	ADCON1		; 
	movlw	0x2
	movwf	ADCON1		; make A0-1 A/D, A2-3 digital
	banksel	ADCON0		;
	movlw	0xC1		; RC clock, enable AD, selects chan 0
	movwf	ADCON0		; set the channel to 0
	banksel	ADRES
	clrf	ADRES		; clr the conversion buffer
	return

;
; Check the a to d convertor, called from a tight poll loop
;
checka2d		
	global	checka2d
	banksel	a2dstate
	movlw	high doSetup
	movwf	PCLATH
	movf	a2dstate,w	; get the current state for a to d conv
	addwf	PCL,f		; do a jump to the state handler
	goto	doSetup
	goto	doSetupWait
	goto	doStartConv
	goto	doWaitForConv

doSetup
	banksel	a2dstate
	incf	a2dstate,f	; set the next state (setupWait)
	movf	a2dchan,w	; get the new channel
	movwf	tmp1		; save it in a temp variable 
	swapf	tmp1,f		; swap into high nibble and then
	srf	tmp1,f		; shift it into the channel spec position
	movlw	0xC1		; mask for the clock and a/d enable
	iorwf	tmp1,w		; or these bits into the channel
	banksel	ADCON0
	movwf	ADCON0		; set the new channel for the a to d
	return

doSetupWait
	banksel	a2dstate
	incf	a2dstate,f	; just a wheel spin state, goto Start Conv
	return

doStartConv
	banksel	a2dstate
	incf	a2dstate,f	; set the next state WaitForConv
	banksel	ADCON0
	bsf	ADCON0,GO_DONE	; Start the conversion GO is active high
	return

doWaitForConv
	banksel	a2dstate
	banksel ADCON0
	btfsc	ADCON0,GO_DONE	; clear means ready, set means not yet
	return			; not ready
	banksel	a2dstate
	clrf	a2dstate	; clear the state to setup
	movf	a2dchan,w	; get the current a to d channel
	addlw	a2dvals		; add it to the base array addr
	movwf	FSR		; set up the indirect point
	banksel	ADRES
	movf	ADRES,w		; get the a/d value
	banksel	a2dvals
	movwf	INDF		; save it at the current pointer location
	movf	a2dchan,w	; get the current channel
	call	getBit		; set w to the mask bit
	iorwf	a2dflags,f	; set the flag for the chan to say it's ready
bmpChan
	incf	a2dchan,f	; bump to next channel
	movlw	NumChans	; get the max val
	subwf	a2dchan,w	; test for max val
	btfsc	STATUS,Z	; is it max
	clrf	a2dchan		; yes, reset
	return			; no, go to next state (setup)

	table	getBit
	retlw	0x01
	retlw	0x02
	retlw	0x04
	retlw	0x08
	retlw	0x10
	retlw	0x20


	; get the a2d value, 
	; w has the a2d number
	; w returns with value
geta2dval
	global	geta2dval
	banksel a2dvals
	addlw	a2dvals		; calculate the addr of the a2d array
	movwf	FSR		; set a2d pointer
	movf	INDF,w		; get the a2d value
	return

	END
