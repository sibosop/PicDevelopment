
	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#define IN_WALKER
	#include "walker.inc"
	#include "timers.inc"
	#include "../shared/macros.inc"
		list
	


; example of using Uninitialized Data Section
USRFILES       	UDATA   
state		res	1		; run state
tmp1		res	1
tmp2		res	1
tmp3		res	1
tmp4		res	1
dirCnt		res	1

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
		clrf	TRISB
		banksel	TRISA			
		clrf	TRISA
		banksel	PORTA
		clrf	PORTA
		banksel	state
		clrf	state			; clear state machine
		banksel	PORTB
		clrf	PORTB			; clear all ouputs
		bsf	PORTB,0
		call	interinit		; init interrupts
		call	inittimers		; init timers
		stimw	MotorTimer,SwitchTime	; start motor timer (1 second)
		enable				; start the interrupts
resetDirCnt
		movlw	DirLimit
		movwf	dirCnt
loop	
		clrwdt				; feed the doggie
		banksel	state
		btfss	state,Alarm		; 1khz interrupt?
		goto	loop			; no, continue
		bcf	state,Alarm		; yes, clear flag
		call	checktimers		; bump pending timers
		btfsc	timermask,MotorTimer	; see if motor change ready
		goto	loop			; no, check the sensors
setMotor
		stimw	MotorTimer,SwitchTime	; reset timer
		btfss	state,LeftFlag		; are we going left
		goto	lp1			; no try other states
		bcf	state,LeftFlag		; set for right next time
		btfss	state,ForwardFlag	; yes, are we going forward
		goto	lp2			; no try other states
		movlw	ForwardLeft		; yes, get the mask
		goto	setb
lp2
		movlw	BackwardLeft		; get the mask
		goto	setb

lp1
		bsf	state,LeftFlag		; set for left next time
		btfss	state,ForwardFlag	; are we going forward
		goto	lp3			; no try other states
		movlw	ForwardRight		; get the mask
		goto	setb
lp3
		movlw	BackwardRight		; get the mask
setb
		banksel	PORTB		
		movwf	PORTB			; store the new mask
		banksel	dirCnt			
		decf	dirCnt,f		; bump the dir cnt
		btfss	STATUS,Z		; are we done with this dir?
		goto	loop			; no, then continue
		btfss	state,ForwardFlag	; yes, are we going forward
		goto	setForward		; no, the set for forward
		bcf	state,ForwardFlag	; yes, set for backward
		goto	resetDirCnt		; reset the counter
setForward	bsf	state,ForwardFlag	; set for forward
		goto	resetDirCnt


#if 0
checkFront
		btfsc	timermask,FrontDebounce	; is the debounce timer on?
		goto	checkBack		; yes, try back then
		banksel	PORTB			; no, then
		btfsc	PORTB,FrontSensor	; check the sensor
		goto	checkBack		; not set, look at the back
		banksel	state	
		stimw	FrontDebounce,D'1000'	; set the debounce timer
		bcf	state,ForwardFlag	; set the state to go backward
		goto	setMotor		; reset the motors
checkBack
		btfsc	timermask,BackDebounce	; is the debounce timer on?
		goto	loop			; yes, try later
		banksel	PORTB			
		btfsc	PORTB,BackSensor	; check the sensor
		goto	loop			; no? try later
		banksel	state		
		stimw	BackDebounce,D'1000'	; set the back debounce
		bsf	state,ForwardFlag	; set the motrs to to forward
		goto	setMotor		; reset the motors
#endif
		


	
		
		
		END                     ; directive 'end of program'
