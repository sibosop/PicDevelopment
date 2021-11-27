
	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions
	#define IN_VOICEPLY
	#include "voiceply.inc"
	#include "timers.inc"
	#include "a2d.inc"
	#include "../shared/macros.inc"
	#include "math.inc"
		list
	


; example of using Uninitialized Data Section
USRFILES       	UDATA   
state		res	1		; run state

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
		movlw	0xf1			; eom and top 4 bits for rand
		movwf	TRISB
;		banksel	TRISA			
;		movlw	0xf			
;		movwf	TRISA			; make A0-3 inputs A4 output
;		banksel	PORTA
;		clrf	PORTA
		banksel	PORTB
		clrf	PORTB			; clear all ouputs
;		banksel	ADCON1
;		movlw	0x3
;		movwf	ADCON1
		banksel	state
		call	interinit		; init interrupts
		call	inittimers		; init timers
		call	initVoc
		call	initrand
		call	a2dinit		; initialize a to d handlers
		enable				; start the interrupts
		stimw	startupTimer,D'3000'
		clrf	state			; clear state machine
		bsf	state,Startup
loop	
		clrwdt				; feed the doggie
		call	checka2d
		banksel	state
		btfss	state,PudWait
		goto	testEom
		call	checkPud
testEom
		banksel	state
		btfss	state,Eom
		goto	testAlarm
		call	checkEom
		banksel	state
		bcf	state,Eom
testAlarm
		btfss	state,Alarm		; 1khz interrupt?
		goto	loop			; no, continue
		banksel	state
		bcf	state,Alarm		; yes, clear flag
		call	checktimers		; bump pending timers
		btfss	state,Startup
		goto	testVoc
		btfsc	timermask,startupTimer
		goto	loop
		bcf	state,Startup
testVoc
		call	checkVoc
		goto	loop
		END                     ; directive 'end of program'
