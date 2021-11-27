
	processor	16C73B
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include 	minput.inc
	#include	midi.inc
	#include	math.inc
	#include	timers.inc

	LIST

	constant NumChans	= 5

RAM2	UDATA
a2dvals	 res	NumChans	; a to d vals for each chnnel
a2dflags res	1		; flags say whether the value has changed
a2dchan	 res	1		; current a to d channeld
a2dstate res	1		; a to d state machine
cnum	res 1
cmask	res 1
	constant	ControlBase = 4
	
	global	a2dvals
	global	a2dflags

	extern	tmp1		; needed for address conversions

PROG1	CODE

a2dinit
	global	a2dinit
	banksel	a2dflags	
	clrf	a2dflags	; clear the 'changed flags'
	clrf	a2dchan 	; clear the current a to d channel
	clrf	a2dstate	; put the state machin in setup mode
	banksel	TRISA		; set A to inputs
	movlw	0x1F		; maybe not necessary, but why not
	movwf	TRISA		; make port A all inputs
	banksel	ADCON1		; 
	clrf	ADCON1		; make all A2D ports analog inputs
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
	banksel	a2dstate
	clrf	a2dstate	; clear the state to setup
	movf	a2dchan,w	; get the current a to d channel
	addlw	a2dvals		; add it to the base array addr
	movwf	FSR		; set up the indirect point
	banksel	ADRES
	movf	ADRES,w		; get the a/d value
	banksel	tmp1
	movwf	tmp1
	srf	tmp1,w		; use only 0-127
	banksel	a2dvals
	subwf	INDF,w		; has the value changed
	btfsc	STATUS,Z	; zero means no change
	goto	bmpChan
	banksel	ADRES
	movf	ADRES,w		; get the a to d value
	banksel tmp1
	movwf	tmp1
	srf	tmp1,w
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



checkctlr
	global	checkctlr
	banksel	a2dflags
	movf	a2dflags,f	; set by the a2d if a value has changed
	btfsc	STATUS,Z	; test for no values changed
	return			; no? return
	clrf	cnum		; set the control number to 0
	movlw	1		; set the test mask to the first bit
	movwf	cmask		; cmask does double duty as test and counter
ctestlp
	movf	cmask,w		; get the current mask value
	andwf	a2dflags,w	; and with flags to set if value is set
	btfss	STATUS,Z	; zero means nada
	call	sendval		; not zero? then send the new value
	banksel	cmask
	slf	cmask,f		; shift mask to next value		
	btfsc	cmask,5		; bit 5, we've hit the end
	return			; so return
	incf	cnum,f		; not at the end, Bump the control number
	goto	ctestlp		; test the next bit
sendval
	xorlw	0xff		; w has mask, invert it
	andwf	a2dflags,f	; so we can then clear this flag
	movf	cnum,w		; get the control number
	addlw	a2dvals		; make the pointer the pointer offset
	movwf	FSR		; set pointer
	movf	INDF,w		; get value for a/d 
	movwf	tmp2		; save the value for macro
	movf	cnum,w		; get the control number again
	addlw	ControlBase	; add the base offset for the controller
	movwf	tmp1		; save for the macro
	midicntl tmp1,0,tmp2,0	; send the midi control message
	return

	END
