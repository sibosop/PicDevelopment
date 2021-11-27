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
	#include	timers.inc
	#include	rfrecv.inc
	#include	midi.inc


testbit	macro	num
	banksel	timermask
	btfsc	timermask,num		; is the timer running
	goto	tb2#v(num)			; yes then quit
	banksel	notemask		
	btfss	notemask,num		; is the note on
	goto	tb1#v(num)		; no check the port
	bcf	notemask,num		; yes, clr it
	midpnl	D'60' + num, 1, 0, 1	; turn note off
	goto	tb2#v(num)		; done
tb1#v(num)
	banksel	PORTB
	btfss	PORTB,num		; bit high on port
	goto	tb2#v(num)		; no, done
	midpnl	D'60'+ num ,1,D'100',1	; yes , play a note
	banksel	notemask
	bsf	notemask,num		; set that it's playing
	stimw	num, D'1000'		; set timer
tb2#v(num)
	endm
	

; define global registers
RAM1	UDATA
state		res     1

RAM2	UDATA
notemask	res	1


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
	movlw	0xFC		; 
	movwf	TRISB		; make b all inputs except 1&2
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
	call	inittimers	; initialize timers
	banksel	notemask
	clrf	notemask

; The MAIN loop 
main
	banksel	PORTC
	bcf	PORTC,4
	clrwdt			; kick the dog
	btfss	state,alarm	; check for timer alarm
	goto	main
	bcf	state,alarm	; clear timer alarm
	banksel	PORTC
	bsf	PORTC,4
	banksel	state
	call	checktimers
;	testbit 0
;	testbit 1
	testbit 2
	testbit 3
	testbit 4
	testbit 5
	testbit 6
	testbit 7
	goto	main
	
	END
