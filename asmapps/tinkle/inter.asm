; inter.asm
; Interrupt code for ROBOCAR
;  (c) M.Schippling Y2k+1
;
; sub-routine entry point: inter
; called from startup.asm with base registers saved
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\queue.inc"
	#include	tinkle.inc

; define local registers, etal
RAM1	UDATA
tmpcnt	res	1
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
	btfss	INTCON,T0IF	; test timer interrupt
	goto	testut		; no, test for transmit
	bcf	INTCON,T0IF	; clear timer interrupt
	movlw	MILLIT		; shave off some cycles
	movwf	TMR0		;
	decf	milli,f		; dec the millisecond count
	btfss	STATUS,Z	; are we done
	goto	testut		; no, see if we're almost done
	movlw	MILLI		; get the next millisec count
	movwf	milli		; reset the counter
	bsf	state,alarm	; set the alarm for main
testut
	call	chkutrans
testur
	call	chkurecv
	return


interinit
	global interinit
	clrf 	milli
	bsf	milli,0
	return

	END
