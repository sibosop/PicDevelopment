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
	#include	timers.inc
	#include	tweet81z.inc
	#include	a2d.inc



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
	movlw	0x8F		; except for transmit/C4 (debug)
	movwf	TRISC		; of this
	banksel	TRISB
	movlw	0x0		; 
	movwf	TRISB		; make b all outputs
	banksel	OPTION_REG
	movlw	0x80		; divide by 2, disable pullups on B
	movwf	OPTION_REG
	banksel	SSPCON
	bcf	SSPCON,SSPEN	; disable the synchronous serial port
	banksel	PORTA
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
	call	a2dinit		; initialize a to d handlers
	call	inittimers	; initialize timers
	call	initNotes

; The MAIN loop 
main
	banksel	PORTC
	bcf	PORTC,4
	clrwdt			; kick the dog
	btfsc	state,alarm	; check for timer alarm
	goto	doAlarm
	call	checka2d
	goto	main		; do nothing if not set
doAlarm
	bcf	state,alarm	; clear timer alarm
	banksel	PORTC
	bsf	PORTC,4
	banksel	state
	call	checktimers
	call	checkNotes
	goto	main
	END
