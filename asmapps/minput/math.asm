

; math.asm

;

; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_MATH
	#include	P16C73B.INC
	#include	math.inc
	#include	../shared/macros.inc
	

	




; define local registers, etal

; Put the m
RAM2	UDATA
tmp1		res	1	; temp var
tmp2		res	1
tmp3		res	1
tmp4		res	1
	global	tmp1
	global	tmp2
	global	tmp3
	global	tmp4

RAM1	UDATA	

	
randflags	res	1	; various global flags
oscval		res	1	; random oscillator values

	constant compflag = 0

PROG1	CODE

;
; initialize the midi subsystem
;
initrand
	global initrand
	banksel	randflags	; get on right ram page
	clrf	oscval
	call	getrand
	return
	


getrand
	global	getrand
	banksel	oscval
	swapf	oscval,f	; mangle oscval some more
	movlw	0xf0
	andwf	oscval,f	; clear bottom 4 bits
	banksel	PORTC
	movf	PORTC,w		; or in the b port oscillators
	andlw	0xf		; bottom four bits working
	banksel	oscval
	iorwf	oscval,f	; or in with top 4
	movf	oscval,w	; retreive the value
	btfss	randflags,compflag	; should we comp it
	goto	getrand0
	bcf	randflags,compflag	; swap the flag
	comf	oscval,f		; comp the value
	return				; return
getrand0 bsf	randflags,compflag	; flip the flag
	return				; w has the value




;
; one byte integer divide subroutine taken from div macro
; parameters:
;	uses tmp1,tmp2,tmp3,tmp4 as parameters
;	t - temporary register (tmp1)
;	r - return value (tmp2)
;	a - dividee (returns as remainder) tmp3
;	b - divider (tmp4)
;
;div	macro	t,r,a,b
divide
	global	divide
	movlw	1
	movwf	tmp1		; t = 1
divide0
	btfsc	tmp4,6		; while ( (b & 0x40) == 0 )
	goto	divide1		; 
	slf	tmp4,f		; {b << 1
	slf	tmp1,f		;  t << 1
	goto	divide0
divide1
	clrf	tmp2		; r = 0
divide2				; do
	movf	tmp4,w		; if ( a - b ) >= 0 )
	subwf	tmp3,w		; {
	btfsc	STATUS,Z	; != 0
	goto	divide4
	btfss	STATUS,C	; C set on a-b > 0 
	goto	divide3
divide4
	movwf	tmp3		; a = a - b
	movf	tmp1,w		; r = r + t
	addwf	tmp2,f		; }
divide3
	srf	tmp4,f		; b >> 1
	srf	tmp1,f		; t >> 1
	movf	tmp1,f		; 
	btfss	STATUS,Z	; } while ( t != 0 )
	goto	divide2
	return			; r = result, a = remainder
	END
