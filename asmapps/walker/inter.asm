	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#include "walker.inc"


USRFILES	UDATA
w_temp		res	1		; save location for w at interrupt
status_temp	res	1		; save location for status at interrupt
milli		res	1


	constant	MILLI	= D'3'
	constant	MILLIT	= D'99'



PROG	CODE

interrupt	
		global	interrupt
		movwf   w_temp		; save off current W register contents
		movf	STATUS,w	; move status register into W register
		movwf	status_temp	; save off contents of STATUS register

; isr code can go here or be located as a call subroutine elsewhere
		banksel	INTCON
		btfss	INTCON,T0IF
		goto	nexttest
		bcf	INTCON,T0IF	; clear timer interrupt
		movlw	MILLIT
		banksel	TMR0
		movwf	TMR0
		banksel	milli
		decf	milli,f
		btfss	STATUS,Z
		goto	nexttest
		movlw	MILLI
		banksel	milli
		movwf	milli
		bsf	state,Alarm	; set the ALARM state bit 

nexttest
		banksel	status_temp
		movf    status_temp,w	; retrieve copy of STATUS register
		movwf	STATUS          ; restore STATUS register contents
		swapf   w_temp,f
		swapf   w_temp,w	; restore pre-isr W register contents
		retfie			; return from interrupt

interinit
		global	interinit
		banksel	INTCON
		bsf	INTCON,T0IE
		banksel	OPTION_REG
		clrf	OPTION_REG
		banksel	TMR0
		clrf	TMR0		; kick off timer
		banksel	milli
		clrf	milli
		bsf	milli,0
		return

		END
