; main.asm
; Main routine for ROBOCAR
;  (c) M.Schippling Y2k+1
;
; sub-routine entry point: init
; called from startup.asm on reset
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include 	../shared/queue.inc
	#define		isGlob
	#include	moron.inc
	#include 	serial.inc
	#include	midi.inc


; define global registers
	UDATA
state		res	1
cntr		res     1
precnt		res	1
note		res	1


	global	cntr
	global	precnt
	global	state




; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
init
	global init

	banksel	TRISB
	clrf	TRISB		; make b outputs
	banksel	OPTION_REG
	if	(clockrate == 20)		
	movlw	0x4		; divide by 128
	endif
	if 	(clockrate == 10)
	movlw	0x5		; divide by 64
	endif
	if 	(clockrate == 4)
	movlw	0x3		; divide by 16
	endif
	movwf	OPTION_REG
	banksel	precnt
	clrf	precnt
	clrf	cntr
	clrf	state
	clrf	TMR0		
	clrf	note
	clrf	INTCON		; disable all interrupts
	bsf	INTCON,T0IE	; enable timer 0 interrupts
	bsf	INTCON,PEIE	; enable peripheral interrupts
	bsf	INTCON,GIE	; enable general interrupts
	call	uartinit
	call	midiinit

; The MAIN loop thing
main
	banksel	state
	btfss	state,alarm	; check for timer alarm
	goto	main		; do nothing if not set
	bcf	state,alarm	; clear timer alarm
	btfss	state,noteon	; is there a note playing?
	goto	newnote		; no, play a note
	bcf	state,noteon	; yes, clear the on flag
	midpnl	note,0,0,1	; turn off note
	goto	putcount	; go to next part
newnote
	banksel	state
	bsf	state,noteon	; set that a note is playing
	movf	cntr,w		; get counter value
	andlw	0x0f		; lower 4 bits only
	addlw	D'60'		; add to middle c
	movwf	note		; store note
	midpnl	note,0,D'55',1	; send a note on to midi
putcount
	banksel	cntr		
	movf	cntr,w		; get the current cntr
	andlw	0x0f		; use bottom 4 bits
	call	setdigit	; get the led configuration
	banksel	PORTB		; 
	movwf	PORTB		; write the led
	goto	main		; start over

setdigit
	addwf	PCL,f		; add cntr to pc value
	retlw	0x3f
	retlw	0x06
	retlw	0x5b
	retlw	0x4f
	retlw	0x66
	retlw	0x6d	
	retlw	0x7d	
	retlw	0x07
	retlw	0x7f
	retlw	0x6f
	retlw	0x77
	retlw	0x7c
	retlw	0x39
	retlw	0x5e
	retlw	0x79
	retlw	0x71
	END
