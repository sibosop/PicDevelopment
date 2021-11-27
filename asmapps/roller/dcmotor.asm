
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#include	roller.inc
	#define		IN_DCMOTOR
	#include 	dcmotor.inc
	#include 	timers.inc

	LIST	
	
	
RAM1	UDATA
motorState	res	1	; state of the motors
leftCurSpeed	res	1	; current speed of left motor
rightCurSpeed	res	1	; current speed of right motor
leftEncCount	res	1	; target speed of left motor
rightEncCount	res	1	; target speed of right motor

;
; write a value to the motor
; tmp1 has motor number
; tmp2 has motor value
;
PROG1	CODE


;	Hardware motor bits
	constant LeftEnable	= 7
	constant LeftForward	= 6
	constant RightEnable	= 5
	constant RightForward	= 4
	constant RightEncBit 	= 2
	constant LeftEncBit	= 1

; motor timers
	constant OnTimer       = 0
	constant LeftOffTimer  = 1
	constant RightOffTimer = 2
	constant MotorSwapTimer = 3

; state bits
	constant LeftEnabled 	= 0
	constant RightEnabled	= 1
	constant LeftEncOn	= 2
	constant RightEncOn	= 3
	constant LeftOn		= 4
	constant RightOn	= 5
	constant SwapLeft	= 6


; other constants
	constant PulseTime	= D'25'
	constant MotorSwapTime	= D'2'

	

;
; clear the motors 
;
clrMotor
	global	clrMotor
	banksel	motorState
	clrf	motorState
	clrf	leftCurSpeed
	clrf	leftEncCount
	clrf	rightCurSpeed
	clrf	rightEncCount
	call	initTest
	return

;
; This is called every 1khz tick
;
checkMotor
	call	checkSwap
	call	testMotor
	global	checkMotor
	banksel	timermask
	btfsc	timermask,OnTimer	; see it the pulse width has passed
	goto	checkOffTimers		; no, check the off values
	stimw	OnTimer,PulseTime	; yes, reset the freq timer
	btfss	motorState,LeftEnabled	; left motor running
	goto	checkRightOn		; no, check right
	bsf	motorState,LeftOn	; yes, turn on the motor
	stimf	LeftOffTimer,leftCurSpeed,0,tlow	; set the pulse width
checkRightOn
	btfss	motorState,RightEnabled	; is the right motor on
	goto	checkOffTimers		; no check the pulse width off
	bsf	motorState,RightOn	; turn on the right motor
	banksel	rightCurSpeed
	stimf	RightOffTimer,rightCurSpeed,0,tlow 
checkOffTimers
	btfsc	timermask,LeftOffTimer	; has the off time of the pulse started
	goto	checkRightOff		; no, check right
	bcf	motorState,LeftOn	; turn left motor off
checkRightOff
	banksel	timermask
	btfsc	timermask,RightOffTimer ; has the off time of the pulsue started
	return				; no, then we're done
	bcf	motorState,RightOn	; turn the right motor off	
	banksel	timermask
	return

checkEncoder
	global	checkEncoder
	banksel PORTB
	btfss	PORTB,LeftEncBit		; is the left bit on
	goto	doLeftOff			; no check the off
	banksel	motorState
	btfsc	motorState,LeftEncOn		; is the state off
	goto	doRightOn			; no the test the right
	bsf	motorState,LeftEncOn		; yes, set it on 
	incf	leftEncCount,f			; bump the count
	goto	doRightOn			; test the right
doLeftOff
	btfss	motorState,LeftEncOn		; is the state on
	goto	doRightOn			; no, test the right
	bcf	motorState,LeftEncOn		; yes turn is off
doRightOn
	banksel PORTB
	btfss	PORTB,RightEncBit		; is the right bit on
	goto	doRightOff			; no test the right off
	banksel	motorState
	btfsc	motorState,RightEncOn		; is the state off
	return					; no, then we're done
	bsf	motorState,RightEncOn		; yes, set it on
	incf	rightEncCount,f			; bump the count
	return					; were done
doRightOff
	btfss	motorState,RightEncOn		; is the state on?
	return					; no, we're done
	bcf	motorState,RightEncOn		; yes, turn it off
	return					; now we're done

checkSwap
	banksel	timermask
	btfsc	timermask,MotorSwapTimer	; is it swap time
	return					; nope
	stimw	MotorSwapTimer,MotorSwapTime	; reset swap timer
	btfss	motorState,SwapLeft		; check the left?
	goto	doSwapRight			; no check the right
	bcf	motorState,SwapLeft		; check the right next time
	bcf	PORTB,RightEnable		; turn off the right
	btfss	motorState,LeftOn		; is the left on?
	return					; no, leave it off
	banksel	PORTB				
	bsf	PORTB,LeftEnable		; yes turn it on
	banksel	motorState
	return
doSwapRight
	bsf	motorState,SwapLeft		; check the left next time
	bcf	PORTB,LeftEnable		; turn off the left
	btfss	motorState,RightOn		; is the right on?
	return					; no, leave it off
	banksel	PORTB				; yep
	bsf	PORTB,RightEnable		; turn it on
	return
	
	
	

	


	constant TestTimerRight = 4
	constant TestTimerLeft  = 5
initTest
	banksel	leftCurSpeed
	movlw	D'18'			; set the left and right speend
	movwf	leftCurSpeed		; to a constant value
	movwf	rightCurSpeed
	return

testMotor
	banksel	timermask
	btfsc	timermask,TestTimerRight ; has the stop timer fired
	goto	testTimerLeft		 ; no, see if the left has
	stimw	TestTimerRight,D'5000'	; wait for a while
	bsf	motorState,RightEnabled	 ; yes, turn the motor regardless
	movf	rightEncCount,w		 ; see how many spins we've had
	sublw	D'50'			 ; see if it's the max amount
	btfsc	STATUS,C		
	goto	testTimerLeft		; no check left
	clrf	rightEncCount		; yes, clear the count for next time
	banksel	PORTC
	bsf	PORTC,1
	banksel	motorState
	bcf	motorState,RightEnabled	; turn off the motor
testTimerLeft
	btfsc	timermask,TestTimerLeft		; are we waiting
	return					; yes. then we be done
	stimw	TestTimerLeft,D'5000'	; wait a while
	bsf	motorState,LeftEnabled		; no, make sure the motor's on
	movf	leftEncCount,w			; get the rotation count
	sublw	D'50'			; have we hit the max
	btfsc	STATUS,C		; 
	return				; no, then we be done
	clrf	leftEncCount		; yes, clear the count
	bsf	PORTC,2
	bcf	motorState,LeftEnabled	; turn the motor off
	return				; now where done
	


	END
	
	
