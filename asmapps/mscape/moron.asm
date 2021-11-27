

; moron.asm

;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#define		IN_MORON
	#include	moron.inc
	#include	midi.inc
	

	




; define local registers, etal

; Put the m
RAM2	UDATA	

	
; this is an array of the struct
;	byte	timerlo
;	byte	timerhi
;	byte	flags
;	byte	note

	constant	numvoices = D'8'
	constant	voicesize = D'4'

	; offsets
	constant	timerlo   = D'0'
	constant	timerhi	  = D'1'
	constant	flags     = D'2'
	constant	note	  = D'3'

	constant	arraysize = (numvoices * voicesize)
	constant	lownote	  = D'36'	; lowest note
	constant	spread	  = D'6'	; distance between notes
	constant	pcspread	  = D'26'
	constant	basepc	  = D'7'
	constant	minhi	  = 0
	constant	maxhi	  = D'24'

voicearray	res	arraysize
moronflags	res	1	; various global flags
oscval		res	1	; random oscillator values
tmp1		res	1	; temp var
tmp2		res	1
tmp3		res	1
tmp4		res	1
voiceptr	res	1	; current voice
voicebase	res	1	; saves having to calculate it
voicebit	res	1	; current voice num
ledmask		res	1

;
; program change information
;
pccntl		res	1	; program change counter low
pccntm		res	1	; program change counter mid
pccnth		res	1	; program change counter hi
chokecntl	res	1	; choke counter low
chokecnth	res	1	; choke counter hi

	constant	pctimeout = D'120000'	; 2 minutes in millisecs
	constant	choketime = D'15000'


	; flags
	constant	compflag = 0
	constant	noteon  = 0
	constant	choke	= 1

; Put it in ROM PAGE0
	CODE

;
; initialize the midi subsystem
;
moroninit
	global moroninit
	banksel	moronflags	; get on right ram page
	clrf	pccntl
	clrf	pccntm
	clrf	pccnth
	clrf	ledmask
	movlw	voicearray	; get ptr to beginning of array
	movwf	FSR		; store indirect pointer
minitloop
	clrf	INDF		; zero the array entry
	incf	FSR,f		; bump the pointer
	movlw	(arraysize+voicearray)	; get ptr to end of array
	subwf	FSR,w		; text for end
	btfss	STATUS,Z	; done ?
	goto	minitloop	; no, get more
	movlw	voicearray	; get ptr to beginning of array
	movwf	FSR		; store indirect pointer
minitloop2
	call	getrand
	movwf	INDF		; arbitratary startup
	movlw	voicesize
	addwf	FSR,f		; bump the pointer
	movlw	(arraysize+voicearray)	; get ptr to end of array
	subwf	FSR,w		; test for end
	btfss	STATUS,Z	; done ?
	goto	minitloop2	; no, get more
	clrf	oscval		; clear randomizer
	clrf	moronflags	; clear flags
	bsf	moronflags,choke ; set the choke flag
	clrf	chokecntl	; the counters will fire
	clrf	chokecnth	; immediately and the voice will change
	return

playmoron
	global	playmoron
	banksel	voiceptr
	movlw	voicearray
	movwf	voiceptr	; set the the first voice
	movlw	lownote
	movwf	voicebase	; set the original voice base
	clrf	voicebit
	bsf	voicebit,0
playloop
	movf	voiceptr,w	; get pointer to voice array
	movwf	FSR		; set indirection
	movlw	1
	subwf	INDF,f		; decrement the low counter
	btfsc	STATUS,C	; is carry set
	goto	nextvoice	; no, do next voice
	incf	FSR,f		; yes bump pointer to hi counter
	subwf	INDF,f		; no, then decrement high counter
	btfsc	STATUS,C	; is carry set
	goto	nextvoice	; no, do next voice
moronready
	call	getrand
	decf	FSR,f		; pointer to timer low
	movwf	INDF
	call	getrand
	andlw	0x7f
	movwf	tmp3
	movlw	maxhi
	movwf	tmp4
	div	tmp1,tmp2,tmp3,tmp4
	movf	tmp3,w
	addlw	minhi
	incf	FSR,f		; pointer to timer high
	movwf	INDF
	incf	FSR,f		; move pointer to flags
	btfss	INDF,noteon	; is there a note playing?
	goto	newnote		; no, play a note
	comf	voicebit,w	; turn off led
	andwf	ledmask,f
	bcf	INDF,noteon	; yes, clear the on flag
	incf	FSR,f		; move pointer to note
	movf	INDF,w
	movwf	tmp1
	midpnl	tmp1,0,0,1	; turn off note
	goto	nextvoice
newnote
	bsf	INDF,noteon	; set that a note is playing
	btfsc	moronflags,choke
	goto	noled
	movf	voicebit,w	; turn on led
	iorwf	ledmask,f
noled
	incf	FSR,f		; move pointer to note
	call	getrand
	andlw	0x7f
	movwf	tmp3
	movlw	spread
	movwf	tmp4
	div	tmp1,tmp2,tmp3,tmp4
	movf	tmp3,w
	addwf	voicebase,w	; get the base of spread
	movwf	INDF
	call	getrand
	btfsc	moronflags,choke
	andlw	0x00
	andlw	0x7f		; velocity is 0 - 127
	movwf	tmp2
	movf	INDF,w
	movwf	tmp1			; can't use INDF with the macros
	midpnl	tmp1,0,tmp2,0	; send a note on to midi
nextvoice
	banksel	voiceptr
	slf	voicebit,f	; set to next voice bit
	movlw	voicesize	; offset to next voice
	addwf	voiceptr,f	; add it to the pointer
	movf	voiceptr,w	; get the pointer
	sublw	(arraysize+voicearray) ; see if we are finished	
	btfsc	STATUS,Z	; are we done
	goto	setleds		; yes return if done
	movlw	spread
	addwf	voicebase,f	; get current voice base
	goto	playloop	; play next note

setleds
	banksel	ledmask
	movf 	ledmask,w
	andlw	0xf
	banksel	PORTA
	movwf	PORTA
	banksel	ledmask
	movf	ledmask,w
	andlw	0xf0
	banksel	PORTB
	movwf	PORTB
	return
	


;
; Check to see if it's time for a program change
pccheck
	global	pccheck
	banksel	moronflags
	btfss	moronflags,choke
	goto	testpc
	movlw	1
	subwf	chokecntl,f
	btfsc	STATUS,C
	return
	subwf	chokecnth,f
	btfsc	STATUS,C
	return
	bcf	moronflags,choke
	goto	pcchange
testpc
	movlw	1		; decrement the counter
	subwf	pccntl,f
	btfsc	STATUS,C
	return			; not ready
	subwf	pccntm,f
	btfsc	STATUS,C
	return			; not ready
	subwf	pccnth,f
	btfsc	STATUS,C
	return			; not ready
	bsf	moronflags,choke	; choke the player
	movlw	(choketime & 0xff)	; for a amount of time
	movwf	chokecntl		; to let the voices
	movlw	(choketime / 0x100)	; clear
	movwf	chokecnth		; out
	return
pcchange
	midiaoff			; turn all notes off
	banksel	pccntl
	movlw	(pctimeout & 0xff)
	movwf	pccntl
	movlw	((pctimeout & 0x00ff00) / 0x100 )
	movwf	pccntm
	movlw	(pctimeout / 0x10000) 
	movwf	pccnth
	call	getrand
	andlw	0x7f
	movwf	tmp3
	movlw	pcspread
	movwf	tmp4
	div	tmp1,tmp2,tmp3,tmp4
	movf	tmp3,w
	addlw	basepc
	movwf	tmp1
	midpc	tmp1,0		
	return
	
	


getrand
	banksel	oscval
	swapf	oscval,f	; mangle oscval some more
	movlw	0xf0
	andwf	oscval,f	; clear bottom 4 bits
	banksel	PORTB
	movf	PORTB,w		; or in the b port oscillators
	andlw	0xf		; bottom four bits working
	banksel	oscval
	iorwf	oscval,f	; or in with top 4
	movf	oscval,w	; retreive the value
	btfss	moronflags,compflag	; should we comp it
	goto	getrand0
	bcf	moronflags,compflag	; swap the flag
	comf	oscval,f		; comp the value
	return				; return
getrand0 bsf	moronflags,compflag	; flip the flag
	return				; w has the value
	END


