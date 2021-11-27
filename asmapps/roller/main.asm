; main.asm
;
; define chip to be used and include the standard stuff
	processor	16C73B
	#define		isGlob
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#include	timers.inc
	#define		IN_ROLLER
	#include 	roller.inc
	#include 	dcmotor.inc
	LIST



; define global registers
RAM1	UDATA
state	res     1
dircnt	res	1
tmp1	res	1
tmp2	res	1
tmp3	res	1
tmp4	res	1

	global	state
	global	tmp1
	global	tmp2
	global	tmp3
	global	tmp4

	extern	interinit




; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
init
	global init
	banksel	TRISC		; make c inputs
	movlw	0x80		; except for transmit/C4 (debug)
	movwf	TRISC		; of this
	banksel	PORTB
	clrf	PORTB
	banksel	TRISB
	movlw	0x0F		; 
	movwf	TRISB		; make bits 0-3 inputs 4-7 outputs
	banksel	OPTION_REG
	movlw	0x0		; divide by 2
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
	bsf	INTCON,GIE	; enable general interrupts
	call	interinit	; initialize interrupt routines
	call	inittimers	; initialize timers
	call	clrMotor
	banksel	dircnt
	clrf	dircnt
	banksel	PORTC
	movlw	0x55
	movwf	PORTC
	enable

; The MAIN loop 
main
	banksel	PORTC
	bcf	PORTC,0
	bcf	PORTC,4
	clrwdt			; kick the dog
	call	checkEncoder
	banksel	state
	btfss	state,Alarm	; check for timer Alarm
	goto	main		; do nothing if not set
	bcf	state,Alarm	; clear timer Alarm
	banksel	PORTC
	bsf	PORTC,4
	call	checktimers
	call	checkMotor
	goto	main
	


	
	END
