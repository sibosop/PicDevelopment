
	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#define IN_MOTODIM
	#include "motodim.inc"
	#include "timers.inc"
	#include "a2d.inc"
	#include "../shared/macros.inc"
	#include "math.inc"
		list
	


; example of using Uninitialized Data Section
USRFILES       	UDATA   
state		res	1		; run state
step		res  	1
motorcnt	res	1
motoState	res	1

	constant	motorTimer=1
	constant	offTimer=2
	constant	turnCnt=D'48'
	constant	startupDelay=D'120'
	constant	stepDelay=D'1000'
	constant	dir = 0
	constant	startup=1

		global	state

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
	movlw	0xf0			; bottom 4 bits motor
					; top 4 bits for rand
	movwf	TRISB
	banksel	PORTB
	clrf	PORTB			; clear all ouputs
	banksel	state
	call	interinit		; init interrupts
	call	inittimers		; init timers
	call	initrand
	call	a2dinit		; initialize a to d handlers
	call	clrMotor
	enable				; start the interrupts
	clrf	state
loop	
	clrwdt				; feed the doggie
	banksel	state
	call	checka2d
	banksel PORTA
	bcf	PORTA,3
	banksel	state
	btfss	state,Alarm		; 1khz interrupt?
	goto	loop			; no, continue
	banksel	PORTA
	bsf	PORTA,3
	banksel	state
	bcf	state,Alarm		; yes, clear flag
	call	checktimers		; bump pending timers
	call	checkMotor
	btfsc	timermask,offTimer
	goto	loop
	banksel	PORTB
	clrf	PORTB
	goto	loop

clrMotor				; initialize motor program
	global	clrMotor
	banksel	motorcnt		
	clrf	motorcnt
	clrf	step	
	clrf	motoState
	bsf	motoState,startup	; put in startup state
	return

checkMotor
	global	checkMotor
	banksel	timermask
	btfsc	timermask,motorTimer	; timer fired?
	return				; no, return
	btfsc	motoState,startup	; are we in startup mode
	goto	setStartupTime		; yes set startup delay
	stimf	motorTimer,0xff,1,tlow
	movlw	0
	call	geta2dval
	movwf	tmp4
	srf	tmp4,f
	srf	tmp4,f
	stimf	motorTimer,tmp4,0,thigh
cm1
	call	doStep			; step the motor
	stimw	offTimer,D'75'
	incf	motorcnt,f		; bump the number of steps
	movlw	turnCnt			; get the max steps
	subwf	motorcnt,w		; test, are we there
	btfss	STATUS,Z		; 
	return				; not yet
	clrf	motorcnt		; yes, clear the count
	btfss	motoState,dir		; test the direction
	goto	setDir			; change the direction for next
	bcf	motoState,dir		; iteration of steps
testState
	btfss	motoState,startup	; are we in startup?
	return				; no. then we're done
	btfsc	motoState,dir		; have we step forward and back
	return				; no, then do another iteration
	bcf	motoState,startup	; yes, clear startup mode
	return				; we're done with startup mode

setDir
	bsf	motoState,dir		; set the dir bit to change direction
	goto	testState

setStartupTime
	stimw	motorTimer,startupDelay	; set the one time through startup time
	goto	cm1




doStep
	movf	step,w			; get current step count
	btfss	motoState,dir		; figure out our directions
	goto	getBBits		; get the 'backward' bits
	call	getBitsF		; get the 'forward' bits
	goto	setBits			; send bits to motor
getBBits
	call	getBitsB		; get the 'backward' bits
setBits
	banksel	PORTB	
	movwf	PORTB			; send bits to motor
	banksel	step
	incf	step,f			; bump the step for nect time
	btfss	step,2			; 4 phase motor, 4 steps
	return				; we're not at next phase
	clrf	step			; we're at next phase so start again
	return				; done

; double bits for more power
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

		END                     ; directive 'end of program'
