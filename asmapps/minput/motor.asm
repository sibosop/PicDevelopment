
	processor	16C73B
	#include	P16C73B.INC
	#include	../shared/macros.inc
	#define		IN_MOTOR
	#include 	motor.inc
	#include 	math.inc

	LIST	

	constant motorRunning = 0
	constant numMotors    = 4

RAM2	UDATA
motorstate	res  1
motorcnt	res  numMotors
;
; write a value to the motor
; tmp1 has motor number
; tmp2 has motor value
;
PROG1	CODE
	table	getBits
	retlw	0x01
	retlw	0x02
	retlw	0x04
	retlw	0x08
	retlw	0x10
	retlw	0x20
	retlw	0x40
	retlw	0x80

wrtMotor 
	global	wrtMotor
	banksel	tmp1
	movlw	0xf
	andwf	tmp1,f		; make sure motor is legal
	movf	tmp2,w		; get the motor value
	andlw	0xf		; mask out any spurious bits
	iorlw	0xf0		; set the no motor address
	banksel PORTB
	movwf	PORTB		; write bits to latches without setting
	andlw	0x0f		; mask out no motor bit
	banksel	tmp1
	swapf	tmp1,f		; put motor number in high nibble
	iorwf	tmp1,w		; or in the motor number
	banksel	PORTB
	movwf	PORTB		; latch in the bits
	iorlw	0xf0		; set the no motor address
	banksel	PORTB
	movwf	PORTB		; disable the latch again
	return

;
; clear the motors 
; trashs tmp1, tmp2, tmp3
;
clrMotor
	global	clrMotor
	banksel	motorstate
	clrf	motorstate
	clrblk	motorcnt,numMotors
	clrf	tmp3
	clrf	tmp2
clrlp
	movf	tmp3,w
	movwf	tmp1
	call	wrtMotor
	incf	tmp3,f
	movlw	0xe
	subwf	tmp1,w
	btfss	STATUS,Z
	goto	clrlp
	return

checkMotor
	global	checkMotor
	btfss	motorstate,motorRunning
	return

	movlw	0
	movwf	tmp1
	call	bumpmotor
	movlw	0x11
	movwf	tmp1
	call	bumpmotor
	movlw	0x02
	movwf	tmp1
	call	bumpmotor
	movlw	0x13
	movwf	tmp1
	call	bumpmotor
	return




; bump motor takes in tmp1 the motor number
; in the low 4 bits, a the direction in bit
; 4
bumpmotor
	movf	tmp1,w			; get the motor number
	movwf	tmp3			; save it
	andlw	0xf			; mask out the direction bit
	movwf	tmp1			; save motor number
	addlw	motorcnt		; compute the motor cnt addr
	movwf	FSR			; save the pointer
	movf	INDF,w			; get the current count
	andlw	0x3			; roll it over if necessary
	call	getBits			; get the step bit
	movwf	tmp2			; save the step bit
	call	wrtMotor		; write the motor 
	banksel	motorcnt		
	btfss	tmp3,MOTOR_DIR_BIT	; determine direction
	goto	bdown			; clear, then down
	incf	INDF,f			; increment the motor count
	return
bdown
	decf	INDF,f			; decrement the motor count
	return

contMotor
	global	contMotor
startMotor
	global	startMotor
	bsf	motorstate,motorRunning
	clrf	motorcnt
	return

stopMotor
	global	stopMotor
	bcf	motorstate,motorRunning
	return
	END
	
	
