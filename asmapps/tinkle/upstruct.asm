
; upstruct.asm

;

; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_UPSTRUCT
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include	tinkle.inc
	#include	upstruct.inc
	#include	midi.inc
	#include	math.inc
	#include	timers.inc
	#include 	phrases.inc
	

	




; define local registers, etal

; Put the m
OVR1	UDATA_OVR
	constant	lowest  = D'44'
	constant	highest = D'58'
	constant	default	= D'48'
	constant	spread = D'6'
	constant	tritone = D'6'
	constant	majorthird = D'4'
	constant	minorthird = D'3'
	constant	wholestep  = D'2'
	constant	octave	 = D'12'
	constant	waittimemax = D'4000'
	constant	waittimemin = D'2000'
	constant	waittimeinterval = (waittimemax - waittimemin)
	constant	lovel	= D'10'
	constant	hivel	= D'50'
	constant	maxdiff	= D'30'
	constant	upveldiff	= D'20'
	constant	bassvellow = D'30'
	constant	bassvelhigh = D'50'

	constant	ulminnote = D'66'
	constant	ulmaxspread = D'8'
	constant	ulminvel = D'30'

	constant	uptimer 	= 0
	constant	basstimer	= 1

	constant	needchord	= 0
	constant	needphrase	= 1
	constant	neednote	= 2
	constant	needendnote	= 3

	constant	tempo	     = D'60'


upstate		res	1
cnote		res	1
culnote		res	1
cupnote		res	1
cvel		res	1
ulnote		res	1
ulvel		res	1
ulbase		res	1
bassnote	res	1
bassvel		res	1





	
PROG1	CODE


dvnum	set 0
dovoice	macro	root,ceiling,result
	movf	root,w
	movwf	result		; save result in case nothing need to be done
	subwf	ceiling,w	; subtract root from ceiling
	btfsc	STATUS,C	; carry clear if root above ceiling
	goto	__dv#v(dvnum)	; root below ceiling do nothing
	movlw	octave		; root above ceiling move it
	subwf	result,f	; down an octave
__dv#v(dvnum)
dvnum ++
	endm
	

upstructinit
	global	upstructinit
	settempo tempo
	banksel	cnote
	clrf	cnote
	clrf	upstate
	bsf	upstate,needchord
	return




upsoff
	banksel	cnote
	midpnl	cnote,0,0,1	; send a note off to midi
	banksel	cnote
	movlw	tritone		
	addwf	cnote,w
	movwf	tmp1
	midpnl	tmp1,0,0,1	; turn off tritone
	banksel	cupnote
	movf	cupnote,w	; get upper structure note
	clrf	cvel		; turns off the chord
	call	playmajor
	banksel	cnote
	return

upson
	call	getrand
	andlw	0x7f		; mask off high bit of random for div
	banksel	tmp3
	movwf	tmp3		; store for div
	movlw	spread		; get the range of the next note
	movwf	tmp4		; store for div
	call	divide		; offset val in tmp3
	movf	cnote,w		; get the current cnote value
	movwf	tmp1		; store it for a while
	movlw	(spread >> 1)	; get half the spread for next note offset
	subwf	cnote,f		; subtracting this will put the next note
	movf	tmp3,w		; somewhere in the spread
	addwf	cnote,f		; calculate next note
	movf	tmp1,w		; get old cnote value
	subwf	cnote,w		; do a subtract for equally test
	btfss	STATUS,Z	; is it the same
	goto	rangetest	; no, the do range test
	incf	cnote,f		; yes, then bump it up a half step
	goto	playit
rangetest
	movlw	lowest		; get low range value
	subwf	cnote,w		
	btfsc	STATUS,C	; carry set, then are okay on low end
	goto	testhigh	; okay, test the high range
	movlw	default		; too low
	movwf	cnote		; reset the note
	goto	playit
testhigh
	movlw	highest		; get high range value
	subwf	cnote,w
	btfss	STATUS,C	; carry clear then were okay on high end
	goto	playit		; okay, test the low range
	movlw	default		; too high
	movwf	cnote		; reset the note
playit	
	call	getrand		; get random value for velocity
	andlw	0x7f	
	banksel	tmp3
	movwf	tmp3
	movlw	(hivel-lovel)	; velocity spread
	movwf	tmp4
	call	divide	
	movf	tmp3,w
	addlw	lovel
	movwf	cvel
	midpnl	cnote,0,cvel,0
	call	getrand			; calculate a lower velocity for 
	andlw	0x7f
	banksel	tmp3			; higher note
	movwf	tmp3			; get random num
	movlw	maxdiff			; greatest velocity difference
	movwf	tmp4			; save for div
	call	divide
	movf	tmp3,w			; get the vel difference
	subwf	cvel,w			; subtract and save
	movwf	tmp2			; in tmp2 
	movlw	tritone			; make the note a tritone higher
	addwf	cnote,w
	movwf	tmp1
	midpnl	tmp1,0,tmp2,0	; play tritone

;
;	at this point we calculate the "upper structure"
;
	call	getrand
	andlw	0x7f
	banksel	tmp3
	movwf	tmp3
	movlw	upveldiff		; upper velocity max difference
	movwf	tmp4
	call	divide
	movf	tmp3,w			; get the difference
	addwf	cvel,f			; add to the bass velocity
	movf	ulnote,w
	movwf	culnote			; save current melody note
	call	getrand
	andlw	1
	btfss	STATUS,Z
	goto	hichord
;
;	play the "upper stucture" one whole below the bass
;
	banksel	cnote
	movlw	wholestep		; go down a whole step
	subwf	cnote,w
	goto	playmajor
hichord
	banksel	cnote
	movlw	(tritone-wholestep)	; down a whole step from top note
	addwf	cnote,w
playmajor
	banksel	cnote
	movwf	cupnote
	addlw	octave			; bump note up an octave
	movwf	tmp1
	dovoice	tmp1,culnote,tmp2
	midpnl	tmp2,0,cvel,0
	banksel	tmp1
	movlw	majorthird		; next note of chord
	addwf	tmp1,f
	dovoice	tmp1,culnote,tmp2
	midpnl	tmp2,0,cvel,0
	banksel	tmp1
	movlw	minorthird
	addwf	tmp1,f
	dovoice	tmp1,culnote,tmp2
	midpnl	tmp2,0,cvel,0
	banksel	cnote
	return

	

checkupstruct
	global	checkupstruct
	banksel	upstate
	btfss	timermask,uptimer	; see if in wait
	goto	doup
	btfss	timermask,basstimer
	goto	dobass
	return
doup
	btfss	upstate,needchord	; clear? see if we need a chord
	goto	checkphrase		; no, see if we need a line
	bcf	upstate,needchord	; we don't need a new one
	bsf	upstate,needphrase	; we do need a new line
	randnum	0,5,1			; decide whether to play chord	
	banksel	tmp1			; weight toward yes decision
	btfsc	STATUS,Z		; 0 skip playing the chord
	goto	checkupstruct		; 0 - do a phrase
	call	upsoff			; yes, turn it off
	call	upson			; >0 let's turn chord on
	goto	setuptime		; set a wait
checkphrase
	btfss	upstate,needphrase	; do we need a new phrase
	goto	checknote		; no check notes
	clrf	phraseptr		; clear the phrase pointer for new phr
	call	getphrasecount		; get the number of phrases
	movwf	tmp4			; save for divide
	call	getrand			; get a random number
	andlw	0x7f			; div only 7 bits
	banksel	tmp3			; 
	movwf	tmp3			; save for mod
	call	divide			; do the divide (mod)
	movf	tmp3,w			; save the mod
	movwf	phrasenum		; set random phrase number
	randnum	ulminnote,(ulminnote+ulmaxspread),1
	movwf	ulbase
	bcf	upstate,needphrase	; clear the new phrase flags
	bsf	upstate,neednote	; we now need a note
	goto	checkupstruct		; pretend we are just starting

checknote
	btfsc	upstate,neednote	; see where are in the note
	goto	phrasenote		; play the next note in
checkend
	btfsc	upstate,needendnote	; see if we need to end the note
	goto	endnote			; finish the note
bloop
	bsf	upstate,needchord	; blooper
	goto	checkupstruct		; play chord and hope for the best


setuptime
;	call	getrand			; get a random num
;	banksel	tmp1
;	movwf	tmp1
;	stimf	uptimer,tmp1,0,tlow
;	movlw	high __whole
	movlw	low __quart
	movwf	tmp1	
	stimf	uptimer,tmp1,0,tlow
;	movwf	tmp1
;	stimf	uptimer,tmp1,0,thigh
	goto	checkupstruct

;
; play the next note in phrase
;
phrasenote
	jphrl				; get low byte of description
	movwf	tmp1			; save for later
	andlw	0x1f			; low nibble has note spec
	sublw	eos			; are we done with phrase
	btfss	STATUS,Z		; should be zero is yes
	goto	interpphrase		; no, interpret the phrase
	bcf	upstate,neednote	; no  more notes
	bsf	upstate,needchord	; play a chord
	goto	checkupstruct		; back to the top againt

interpphrase
	movf	tmp1,w			; mask for note value
	andlw	0x1f
	sublw	_choose			; is it a rest note
	btfsc	STATUS,Z		; test
	call	choose			; yep, the just wait (keep note set)
normalnote
	movf	tmp1,w			; make for note value
	andlw	0x1f			; get note value
	addwf	ulbase,w		; add to minimun offset
	movwf	ulnote			; store note to be played
	swapf	tmp1,f			; get the velocity values
	srf	tmp1,w			; shift to the end
	andlw	0x7
	call	vels
	movwf	ulvel
	midpnl	ulnote,0,ulvel,0	; play the note
	banksel	upstate
	bcf	upstate,neednote
	bsf	upstate,needendnote	; after playing note, we'll need and end
setwait
	jphrh			; get the time specs
newtime
	movwf	tmp1			; save time spec
	andlw	0xf			; clear top half
	sublw	_nowait
	btfsc	STATUS,Z		; is this the null time?
	goto	checkupstruct		; yes, then don't wait
	movf	tmp1,w
	andlw	0xf
	call	getlowticks		; get tick count low
	movwf	tmp2			; save for timer setup
	stimf	uptimer,tmp2,0,tlow	; setup timer
	movf	tmp1,w			; get tick count 
	andlw	0xf			; clear top half
	call	gethighticks		; get the tick count high
	movwf	tmp2			; save for timer setup
	stimf	uptimer,tmp2,0,thigh	; setup the timer
	randnum	0,5,1
	banksel	tmp1
	btfss	STATUS,Z
	goto	checkupstruct
	call	upsoff
	call	upson
	goto	checkupstruct

choose
	randnum	_C,_C+12,1
	movwf	tmp1
	return

endnote
	midpnl	ulnote,0,0,1
	banksel	phraseptr
	jphrh	
	movwf	tmp1			; save for timer setup
	swapf	tmp1,w			; swap in endnote value
	incf	phraseptr,f		; move to next phrase note
	bsf	upstate,neednote	; say we need a note
	bcf	upstate,needendnote	; sya we don't need an endnote
	goto	newtime			; setup the timers

dobass
	randnum	0,5,1			; decide whether to play a pass note
	btfss	STATUS,Z		; 0 is the play
	goto	basstime		; no play, just set timer
	midpnl	bassnote,0,0,1		; turn off any bass note that's playing
	banksel	bassnote		; 
	movlw	((octave * 3)+wholestep)
	subwf	ulnote,w		; get melody note
	movwf	bassnote		; store the bass note
	randnum	0,3,1			; determine the relation to the melody
	movwf	tmp1			; save
	slf	tmp1,w			; multiply result by 2 (whole step)
	addwf	bassnote,f		; step up to choice
	randnum	bassvellow,bassvelhigh,1 ; get a velocity
;	movlw	D'60'
	movwf	bassvel			; save it
	midpnl	bassnote,0,bassvel,0	; play it
	banksel	bassnote
basstime
	stimf	basstimer,__quart,1,tlow ; quart length always
	goto	checkupstruct

	
	
	
	
	
	
	
	
	
JUMPS7	CODE
	table	vels
	retlw 	(D'00' + ulminvel)
	retlw	(D'10' + ulminvel)
	retlw	(D'20' + ulminvel)
	retlw	(D'30' + ulminvel)
	retlw	(D'40' + ulminvel)
	retlw	(D'50' + ulminvel)
	retlw	(D'60' + ulminvel)
	retlw	(D'70' + ulminvel)


	END
