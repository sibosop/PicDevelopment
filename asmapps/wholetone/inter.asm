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
	#include	moron.inc

; define local registers, etal


; Put it in ROM PAGE0
	CODE

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
	incf	precnt,f	; bump the precount
	btfss	STATUS,Z	; test for zero crossing
	goto	testut		; no, look for transmit
	incf	cntr,f		; bump the counter 
	bsf	state,alarm	; set the alarm for main
testut
	call	chkutrans
testur
	call	chkurecv
	return

	END
