
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#define		IN_MOTOR
	#include 	motor.inc
	#include 	math.inc
	#include	timers.inc

	LIST	

	constant	motorTimer=1
	constant	turnCnt=D'48'
	constant	stepDelay=D'240'

RAM2	UDATA
step		res  	1
motorcnt	res	1
dir		res	1
;
; write a value to the motor
; tmp1 has motor number
; tmp2 has motor value
;
PROG1	CODE

;
; clear the motors 
; trashs tmp1, tmp2, tmp3
;
clrMotor
	global	clrMotor
	banksel	motorcnt
	clrf	motorcnt
	clrf	step	
	clrf	dir
	return

checkMotor
	global	checkMotor
	banksel	timermask
	btfsc	timermask,motorTimer
	return
	stimw	motorTimer,stepDelay
	call	doStep
	incf	motorcnt,f
	movlw	turnCnt
	subwf	motorcnt,w
	btfss	STATUS,Z
	return
	clrf	motorcnt
	movlw	1
	xorwf 	dir,f
	return

doStep
	movf	step,w
	btfss	dir,0
	goto	getBBits
	call	getBitsF
	goto	setBits
getBBits
	call	getBitsB
setBits
	banksel	PORTB	
	movwf	PORTB
	banksel	step
	incf	step,f
	btfss	step,2
	return
	clrf	step
	return

	table	getBitsF
	retlw	0x03
	retlw	0x06
	retlw	0x0c
	retlw	0x09

	table	getBitsB
	retlw	0x09
	retlw	0x0c
	retlw	0x06
	retlw	0x03
	END
	
	
