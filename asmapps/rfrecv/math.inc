#ifndef MATH_INC
#define MATH_INC
	ifndef  IN_MATH
	extern	initrand
	extern	getrand
	extern	divide
	extern	tmp1
	extern  tmp2
	extern  tmp3
	extern  tmp4
	endif
	#include	timers.inc

;
; return a random number between min and max
;	if lit flags are true, then the
;	passed numbers are literal
;	This function eats tmp1,tmp2,tmp3,tmp4
;	w has value after call
;
randnum	macro	min,max,lit
	if (lit==1)
	movlw	((max)-(min))
	else
	banksel	min
	movf	min,w
	banksel	max
	subwf	max,w	
	endif
	banksel tmp4
	movwf	tmp4
	call	getrand	
	andlw	0x7f
	banksel	tmp3
	movwf	tmp3
	call	divide
	movf	tmp3,w
	if (lit==1)
	addlw	min
	else
	banksel	min
	addwf	min,w
	endif
	endm
	
	
	

#endif

