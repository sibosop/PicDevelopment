;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#include	serial.inc
	#include 	midi.inc
	#include	timers.inc
	#define		IN_NOTES
	#include	tweet81z.inc
	#include	a2d.inc
	#include	math.inc



; define global registers
RAM2	UDATA
enableFlags	res	1
noteFlags	res	1
noteVals	res	8
notePtr		res	1
noteMask	res	1
tmpNote		res	1
tmpVel		res	1


; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
initNotes
	global initNotes
	clrf	enableFlags
	clrf	noteFlags
	return

checkNotes
	banksel	enableFlags
	global	checkNotes
	call	setEnableFlags	; test the a2d input for change in notes
	movf	enableFlags,w
	banksel	PORTB
	movwf	PORTB
	banksel	enableFlags
	clrf	notePtr		; clear the pointer to the note val array
	movlw	1
	movwf	noteMask
checkloop
	movf	noteMask,w
	andwf	enableFlags,w	; test if this note is currently enabled
	btfss	STATUS,Z	; Zero means no, it's not
	goto	testNote	; do something to the note
	movf	noteMask,w	; if note was on and it's now not
	andwf	noteFlags,w	; enabled then it needs to be turned off
	btfss	STATUS,Z	; see if note is on
	goto	turnNoteOff	; non-zero? then turn it off
nextNote
	incf	notePtr,f	; bump to next note
	slf	noteMask,f	; bump mask to next note
	btfsc	STATUS,C	; has mask rotated in to carry
	return			; yes, then we are done
	goto	checkloop	; no, try next note
	
testNote
	movf	noteMask,w
	andwf	timermask,w	; w still has mask, see if timer is done
	btfss	STATUS,Z	; zero means done (clear)
	goto	nextNote	; try next note
	movf	noteMask,w	; get the note pointer again 
	andwf	noteFlags,w	; see if note is on or off
	btfss	STATUS,Z	; zero if off, so turn no
	goto	turnNoteOff
turnNoteOn
	movf	noteMask,w
	iorwf	noteFlags,f	; set the note status to on 
	movlw	noteVals	; get the address of the note val array
	addwf	notePtr,w	; make a pointer from the note pointer
	movwf	FSR		; set up the pointer
	randnum	D'32',D'40',1	; get a random not val
	movwf	INDF		; set the note val
	movwf	tmpNote		; save temporarily
	randnum	D'100',D'127',1 ; set a random veloctity
	movwf	tmpVel		; save
	midpn	tmpNote,tmpVel,notePtr	; send the note on message
	banksel	tmpVel
	randnum	D'0',D'127',1	; set timer for 10-20 seconds
	movwf	tmpVel
	stimfn 	notePtr,noteMask,tmpVel,tlow
	randnum 0x27,0x3a,1
	movwf	tmpVel
	stimfn	notePtr,noteMask,tmpVel,thigh
	goto	nextNote

turnNoteOff
	movf	noteMask,w
	xorwf	noteFlags,f	; set the note status to off 
	movlw	noteVals	; get the address of the note val array
	addwf	notePtr,w	; make a pointer from the note pointer
	movwf	FSR		; set up the pointer
	movf	INDF,w		; get the note val
	movwf	tmpNote		; save temporarily
	clrf	tmpVel		; clear the velocity
	midpn	tmpNote,tmpVel,notePtr	; send the note on message
	banksel	tmpVel
	randnum	D'0',D'127',1	; set timer for 3 to 5 seconds
	movwf	tmpVel
	stimfn 	notePtr,noteMask,tmpVel,tlow
	randnum 0x3,0x13,1
	movwf	tmpVel
	stimfn 	notePtr,noteMask,tmpVel,thigh
	goto	nextNote

leveltest	macro	v0,v1,v2,v3,v4,v5,v6,v7
	radix	dec
	local	vnum=0
	while	vnum < 8
	movlw	v#v(vnum)
	subwf	tmp2,w
	btfss	STATUS,C
	return
	bsf	enableFlags,#v(vnum)
vnum ++
	endw
	radix	hex
	endm

setEnableFlags
	banksel	enableFlags
	clrf	enableFlags	; clear current state of the flags
	movlw	0		; set w to a2d converter 0
	call	geta2dval	; get the a2d value
	banksel	tmp1
	movwf	tmp1		; store the a2d value
	movlw	0x7f		; get the max value (dark)
	movwf	tmp2		; store it
	srf	tmp1,w		; get rid of LSB for midi (data < 128)
	subwf	tmp2,f		; subtract from max to invert the value
	leveltest 1,16,35,48,64,96,112,122 
	return






	END
