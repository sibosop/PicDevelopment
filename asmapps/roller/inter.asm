; inter.asm
;
; sub-routine entry point: inter
; called from startup.asm with base registers saved
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include 	roller.inc

; define local registers, etal
RAM1	UDATA
milli	res	1


	constant	MILLI	= D'3'
	constant	MILLIT	= D'99'

; Put it in ROM PAGE0
PROG1	CODE

; Interrupt routine, linked into startup
;	startup does:
;		reg saving
;		set RAM bank 0
;		reg restore
;		return from int
;
inter
	global inter
	banksel	INTCON
	btfss	INTCON,T0IF	; test timer interrupt
	goto	testut		; no, test for transmit
	bcf	INTCON,T0IF	; clear timer interrupt
	movlw	MILLIT		; shave off some cycles
	banksel	TMR0
	movwf	TMR0		;
	banksel	milli
	decf	milli,f		; dec the millisecond count
	btfss	STATUS,Z	; are we done
	goto	testut		; no, see if we're almost done
	banksel	milli
	movlw	MILLI		; get the next millisec count
	movwf	milli		; reset the counter
	banksel state
	bsf	state,Alarm	; set the alarm for main
testut
	return


interinit
	global interinit
	banksel	milli
	clrf 	milli
	bsf	milli,0
	return

	END
