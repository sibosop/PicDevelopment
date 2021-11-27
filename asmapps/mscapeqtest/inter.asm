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
	#include	"..\shared\macros.inc"
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
	bcf	INTCON,T0IF
	incf	precnt,f
	btfss	STATUS,Z
	goto	notYet
	incf	cntr,f
	enqueue	testqueue,cntr,0,0,0
notYet
	return


	END
