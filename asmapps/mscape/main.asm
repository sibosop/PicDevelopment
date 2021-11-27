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
	#include	../shared/macros.inc
	#define		isGlob
	#include	moron.inc
	#include 	serial.inc
	#include	midi.inc


; define global registers
RAM1	UDATA
state	res     1


	global	state
	extern	interinit
	constant square = 2




; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
init
	global init

	banksel	TRISB
	movlw	0xf		; 
	movwf	TRISB		; make b 0-3 inputs 4-7 outputs
	banksel	OPTION_REG
	movlw	0x0		; divide by 2
	movwf	OPTION_REG
	banksel	PORTA
	clrf	PORTA
	banksel	ADCON1
	movlw	0x6	
	movwf	ADCON1		; convert A to be digital i/o
	banksel	TRISA
	movlw	0xC0
	movwf	TRISA		; make A all outputs
	banksel	state
	clrf	state
	clrf	TMR0		
	clrf	INTCON		; disable all interrupts
	bsf	INTCON,T0IE	; enable timer 0 interrupts
	bsf	INTCON,PEIE	; enable peripheral interrupts
	bsf	INTCON,GIE	; enable general interrupts
	call	interinit
	call	uartinit
	call	midiinit
	call	moroninit

; The MAIN loop thing
main
	clrwdt			; kick the dog
	banksel	state
	btfss	state,alarm	; check for timer alarm
	goto	main		; do nothing if not set
	bcf	state,alarm	; clear timer alarm
	disable
	call	pccheck		; check for program change
	call	playmoron	; play moronscape
	enable
	goto	main		; start over

	END
