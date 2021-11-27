
	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#define IN_DIMMER71
	#include "dimmer71.inc"
	#include "timers.inc"
	#include "a2d.inc"
	#include "../shared/macros.inc"

		list
	


; example of using Uninitialized Data Section
USRFILES       	UDATA   
state		res	1		; run state
tmp1		res	1
tmp2		res	1
tmp3		res	1
tmp4		res	1

		global	state

		global	tmp1
		global	tmp2
		global	tmp3
		global	tmp4
;
;	this macro initializes the two motor structures
;

		
		
		extern interinit


PROG	CODE
main
		global main
; initialization code
		banksel	PORTB			; set up PORTB
		clrf	PORTB			; clear previous
		banksel	TRISB			
		movlw	0x01			; bit 0 is an input
		movwf	TRISB
		banksel	PORTB
		clrf	PORTB			; clear all ouputs
		banksel	state
		call	interinit		; init interrupts
		call	inittimers		; init timers
		call	a2dinit		; initialize a to d handlers
		enable				; start the interrupts
		clrf	state			; clear state machine
loop	
		bcf	PORTB,3
		clrwdt				; feed the doggie
		btfsc	state,Alarm		; 1khz interrupt?
		goto	doAlarm
		call	checka2d
		goto	loop			; no, continue
doAlarm
		bsf	PORTB,3
		bcf	state,Alarm		; yes, clear flag
		call	checktimers		; bump pending timers
		call	testTTimer
		call	testZeroCross
		goto	loop

testTTimer
		btfsc	timermask,triacTimer
		goto	loop
		bsf	PORTB,1
		bcf	state,TriacWait
		return


testZeroCross
		btfss	state,ZeroCross
		goto	testZeroCrossLow
		btfsc	PORTB,0
		return
		bcf	state,ZeroCross
resetZeroCross
		bcf	PORTB,1
		stimw	triacTimer,D'4'
		return
testZeroCrossLow
		btfss	PORTB,0
		return
		bsf	state,ZeroCross
		goto	resetZeroCross


		END                     ; directive 'end of program'
