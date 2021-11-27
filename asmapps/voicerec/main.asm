; main.asm
; Main routine for ROBOCAR
;  (c) M.Schippling Y2k+1
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
	movlw	0x0		; 
	movwf	TRISB		; make b all outputs
	banksel	ADCON1
	bsf	ADCON1,PCFG1	; make channel a digit io
	bsf	ADCON1,PCFG2
	banksel TRISA
	movlw	0x0		; make channel a all outputs
	movwf	TRISA
	banksel	OPTION_REG
	movlw	0x0		; divide by 2
	movwf	OPTION_REG
	banksel	SSPCON
	bcf	SSPCON,SSPEN	; disable the synchronous serial port
	banksel	PORTB
	clrf	PORTB
	banksel	state
	clrf	state
	clrf	TMR0		
	clrf	INTCON		; disable all interrupts
	bsf	INTCON,T0IE	; enable timer 0 interrupts
	bsf	INTCON,PEIE	; enable peripheral interrupts
	bsf	INTCON,GIE	; enable general interrupts
	call	interinit	; initialize interrupt routines
	call	uartinit	; initial uart routines
	call	mparseinit	; initialize midi parser
	call	initVoc
	call	inittimers
	enable

; The MAIN loop 
main
	banksel	PORTC
	bcf	PORTC,4
	bcf	PORTA,3
	clrwdt			; kick the dog
	banksel	state
	btfss	state,pudwait
	goto	checkChar
	call	checkPud
	goto	checkEom
checkChar
	banksel	state
	btfss	state,charready
	goto	checkEom
	bcf	state,charready
	banksel	PORTA
	bsf	PORTA,3	
	call	checkmparse
checkEom
	banksel	state
	btfss	state,eom
	goto	checkAlarm
	call	stopPlayback
	banksel	state
	bcf	state,eom
checkAlarm
	btfss	state,alarm	; check for timer alarm
	goto	main		; do nothing if not set
	bcf	state,alarm	; clear timer alarm
	call	checktimers
	banksel	PORTC
	bsf	PORTC,4
	banksel	state
	goto	main
	END
