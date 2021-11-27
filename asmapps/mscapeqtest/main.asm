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
	#include 	../shared/macros.inc
	#define		isGlob
	#include	moron.inc


; define global registers
	UDATA
cntr		res     1
precnt		res	1
testqueue	res	queue_size


	global	testqueue
	global	cntr
	global	precnt




; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
init
	global init

	bsf	STATUS,RP0	; page 1
	clrf	PORTB		; make b outputs
	movlw	0x7		; divide by 256
	movwf	TMR0
	bcf	STATUS,RP0	; page 0
	initqueue testqueue,0,0
	clrf	precnt
	clrf	cntr
	clrf	TMR0		
	movlw	0xA0
	movwf	INTCON


; The MAIN loop thing
main
	dequeue	testqueue,0,0	; dequeue a number
	btfsc	STATUS,Z	; see if a number is ready
	goto	main
	andlw	0x0f
	call	setdigit
	banksel	PORTB
	movwf	PORTB
	goto	main

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
