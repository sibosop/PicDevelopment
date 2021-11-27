

;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#define		IN_BIRDS
	#include	bird.inc
	#include	midi.inc

	EXPAND
	

	




; define local registers, etal

; Put the m
RAM2	UDATA	

	constant	numbirds = 7
	constant	midibase = D'60'	; midi base note middle c
;
; independent bird structure
;
	constant	birdflag = 0
	constant	btimerlo = 1
	constant	btimerhi = 2
	constant	birdsize = 3
	constant	birdarraysize = (birdsize * numbirds)
	constant	notetime =	D'3000'	; three seconds
	constant	birdactive =	0

	


birdnote	res	1
birdarray	res	birdarraysize
birdptr		res	1
birdinp		res	1
birdmask	res	1
	global	birdinp


	CODE

birdsinit
	global	birdsinit
	banksel	birdinp
	movlw	0xff
	movwf	birdinp
	clrf	birdflag
	movlw	birdarray
	movwf	FSR
binitloop
	clrf	INDF
	incf	FSR,1
	clrf	INDF
	incf	FSR,1
	clrf	INDF
	incf	FSR,1
	movlw	(birdarray + birdarraysize)
	subwf	FSR,w
	btfss	STATUS,Z
	goto	binitloop
	return

checkbirds
	global	checkbirds
	banksel	birdmask
	clrf	birdmask	; set the bird mask to first bird
	bsf	birdmask,1
	movlw	midibase
	movwf	birdnote	; set midi note to first bird
	movlw	birdarray	; get the base of the array
	movwf	birdptr		; save in pointer for convenience
birdloop
	movf	birdmask,w	; get the current mask
	andwf	birdinp,w	; and with the input port reading
	btfss	STATUS,Z	; active low, skip if zero
	goto	nextbird	; no, try next bird
	movf	birdptr,w	; get the pointer to the current bird
	movwf	FSR		; set pointer to bflag
	btfss	INDF,birdactive	; is bird note currently active
	goto	birdon		; if not play bird
	incf	FSR,f		; set pointer to timerlo
	movlw	1		; decrement the timer by one
	subwf	INDF,f		; timer lo test
	btfsc	STATUS,C	; 0 crossing?
	goto	nextbird	; no, try next bird
	incf	FSR,f		; point to timer hi
	subwf	INDF,f
	btfsc	STATUS,C	; 0 crossing
	goto	nextbird	; no, try next bird
	movf	birdptr,w
	movwf	FSR		; set pointer back to bird flag
	bcf	INDF,birdactive ; clear bird active flag
	movf	birdmask,w
	iorwf	birdinp,f	; clear (set high) the bird bit
	midpnl	birdnote,0,0,1	; turn the note off
	goto	nextbird
birdon
	movf	birdptr,w
	movlw	FSR			; set pointer back to bflag
	bsf	INDF,birdactive		; set the active flag
	incf	FSR,f			; move pointer to timerlo
	movlw	(notetime & 0xff)	; set timer lo for note duration
	movwf	INDF
	incf	FSR,f			; move the pointer to timerhi
	movlw	(notetime / 0x100)	; get timer hi duration
	movwf	INDF
	midpnl	birdnote,0,D'127',1

nextbird	
	banksel	birdnote	;
	movlw	birdsize	; get the bird entry size
	addwf	birdptr,f	; move pointer to next bird
	incf	birdnote,f	; move midi note to next bird
	slf	birdmask,f	; move test mask to next bird
	btfss	STATUS,C	; are we at the end?
	goto	birdloop	; no, next bird
	return			; yes, we be done

	END


