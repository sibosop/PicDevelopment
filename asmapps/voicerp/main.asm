; main.asm
;
; sub-routine entry point: init
; called from startup.asm on reset
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#define		isGlob
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#include 	serial.inc
	#include	mparse.inc
	#include	voicerec.inc
	#include	timers.inc



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
	banksel	TRISC		; make c inputs
	movlw	0x8F		; except for transmit (debug)
	movwf	TRISC		; of this
	banksel	TRISB
	movlw	0x21		; bits 1 and 5 are inputs 
	movwf	TRISB		; configure PORTB
	banksel	PORTB
	movlw	0xe
	movwf	PORTB
	movf	PORTB,w		; clear any interrupts
	banksel	ADCON1
	bsf	ADCON1,PCFG1	; make channel a digit io
	bsf	ADCON1,PCFG2
	banksel TRISA
	movlw	0x0		; make channel a all outputs
	movwf	TRISA
	banksel	PORTA
	clrf	PORTA
	banksel	OPTION_REG
	movlw	0x80		; divide by 2, disable pullups
	movwf	OPTION_REG	; set intedg to look for falling edge
	banksel	SSPCON
	bcf	SSPCON,SSPEN	; disable the synchronous serial port
	banksel	state
	clrf	state
	bsf	state,startup
	banksel TMR0
	clrf	TMR0		
	banksel	INTCON
	clrf	INTCON		; disable all interrupts
	bsf	INTCON,T0IE	; enable timer 0 interrupts
	bsf	INTCON,PEIE	; enable peripheral interrupts
	bsf	INTCON,INTE	; enable B0 interrupt 
	bsf	INTCON,RBIE	; enable the B input change interrupt
;	bsf	INTCON,GIE	; enable general interrupts
	call	interinit	; initialize interrupt routines
	call	uartinit	; initial uart routines
	call	mparseinit	; initialize midi parser
	call	initVoc
	call	inittimers
	stimw	eomTimer,D'100'
	enable

; The MAIN loop 
main
	banksel	PORTC
	bcf	PORTC,4
	clrwdt			; kick the dog
	banksel	state
	btfss	state,pudwait
	goto	checkChar
	call	checkPud
	goto	testEom
checkChar
	banksel	state
	btfss	state,charready
	goto	testEom
	bcf	state,charready
	call	checkmparse
testEom
	banksel	state
	btfss	state,eom
	goto	testOverFlow
	call	checkEom
	banksel	state
	bcf	state,eom
testOverFlow
	btfss	state,overflow
	goto	checkAlarm
	call	handleOverflow
	banksel state
	bcf	state,overflow
checkAlarm
	btfss	state,alarm	; check for timer alarm
	goto	main		; do nothing if not set
	bcf	state,alarm	; clear timer alarm
	call	checktimers
	banksel	PORTC
	bsf	PORTC,4
	banksel	state
	btfss	state,startup
	goto	testVoc
	banksel	timermask
	btfsc	timermask,eomTimer
	goto	main
	banksel	state
	bcf	state,startup
	goto	main
testVoc
	call	checkVoc
	goto	main
	END
