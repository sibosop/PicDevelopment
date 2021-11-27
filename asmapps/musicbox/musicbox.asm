;**********************************************************************
;   This file is a basic code template for assembly code generation   *
;   on the PICmicro PIC16C71. This file contains the basic code       *
;   building blocks to build upon.                                    *  
;                                                                     *
;   If interrupts are not used all code presented between the ORG     *
;   0x004 directive and the label main can be removed. In addition    *
;   the variable assignments for 'w_temp' and 'status_temp' can       *
;   be removed.                                                       *                          
;                                                                     *
;   Refer to the MPASM User's Guide for additional information on     *
;   features of the assembler (Document DS33014).                     *
;                                                                     *
;   Refer to the respective PICmicro data sheet for additional        *
;   information on the instruction set.                               *
;                                                                     *
;   Template file assembled with MPLAB V3.99.18 and MPASM V2.15.06.   *
;                                                                     *
;**********************************************************************
;                                                                     *
;    Filename:	    xxx.asm                                           *
;    Date:                                                            *
;    File Version:                                                    *
;                                                                     *
;    Author:                                                          *
;    Company:                                                         *
;                                                                     * 
;                                                                     *
;**********************************************************************
;                                                                     *
;    Files required:                                                  *
;                                                                     *
;                                                                     *
;                                                                     *
;**********************************************************************
;                                                                     *
;    Notes:                                                           *
;                                                                     *
;                                                                     *
;                                                                     *
;                                                                     *
;**********************************************************************


	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions

	__CONFIG   _CP_OFF & _WDT_OFF & _RC_OSC 

; '__CONFIG' directive is used to embed configuration data within .asm file.
; The lables following the directive are located in the respective .inc file.
; See respective data sheet for additional information on configuration word.



; example of using Uninitialized Data Section
INT_VAR       	UDATA     0x0C   
w_temp		res	1		; save location for w at interrupt
status_temp	res	1		; save location for status at interrupt
state		res	1		; run state
;delayreg	res	1		; delay register
motorptr	res	1		; pointer to motor struct
stepbuf		res	1		; buffer for final port write
motor1		res	5		; motor structure one	
motor2		res	5		; motor structure two
xorm		res	1		; xor mask



;
; These are the offsets for the motor structure
;
; cureent motor delay
MOTDEL		EQU	0
;current motor count
MOTCNT		EQU	1
;current motor stepcount
STPCNT		EQU	2
;current motor rotcount
ROTCNT		EQU	3
;motor step position
MOTSTP		EQU	4
; motor delay table index
;MOTDELIDX	EQU	5

; number of steps per rot cycle
CYCLECNT	EQU	D'200'
NUMROTS		EQU 	D'30'		

; state bits
ALARM		EQU	0		; timer alarm

;
;	this macro initializes the two motor structures
;

		





STARTUP CODE
;**********************************************************************
reset		goto    main             ; go to beginning of program
		nop			; fill space till interrupt
		nop			; vector
		nop

interrupt	movwf   w_temp		; save off current W register contents
PROG	CODE
		movf	STATUS,w	; move status register into W register
		movwf	status_temp	; save off contents of STATUS register

; isr code can go here or be located as a call subroutine elsewhere
		bsf	state,ALARM	; set the ALARM state bit 
		bcf	STATUS,5	; page 0
		bcf	INTCON,T0IF	; clear timer interrupt
		movf    status_temp,w	; retrieve copy of STATUS register
		movwf	STATUS          ; restore STATUS register contents
		swapf   w_temp,f
		swapf   w_temp,w	; restore pre-isr W register contents
		retfie			; return from interrupt
		
main
; initialization code
		bsf	STATUS,5	; page 1
		clrf	PORTB		; make all outputs
		movlw	0x3
		movwf	ADCON0		; make all digital inputs
		movlw	0xf
		movwf	PORTA		; make all digital inputs
		movlw	0x1		; divide by 4
		movwf	TMR0		; 
		bcf	STATUS,5	; page 0
		movlw	motor1		; initialize motor1
		movwf	motorptr
		call	initmotor
		movlw	motor2		; initialize motor2
		movwf	motorptr
		call	initmotor
		movlw	0xA0		; set general interrupt and tmr0 int
		movwf	INTCON
		clrf	TMR0		; kick off timer
		clrf	xorm		; clear the xor mask
loop	
		btfss	state,ALARM	; test for interrupt alarm
		goto	loop		; loop if no alarm
		bcf	state,ALARM	; clear the alarm bit
		clrf	stepbuf
		movlw	motor1		; get pointer to motor1
		movwf	motorptr	; save pointer for subroutine
		call	checkmotor	; determine step value
		movwf	stepbuf
		movlw	motor2
		movwf	motorptr	
		call	checkmotor
		swapf	stepbuf,f
		iorwf	stepbuf,w
		movwf	PORTB
		goto	loop
		
;;
;;	subroutines
;;
initmotor	
		movf	motorptr,w	; get pointer to motor structure
		movwf	FSR		; set to current delay count
		clrf	INDF		; delay count to 0 (it will be reset)
		incf	FSR,f		; move pointer to count
		movlw	0x1
		movwf	INDF		; set count to fire on first call
		incf	FSR,f		; move pointer to step count
		movwf	INDF		; set step count to fire on first call
		incf	FSR,f		; move pointer to rot count
		movwf	INDF		; set rot count to fire on first call
		incf	FSR,f		; move pointer to rot position
		clrf	INDF		; step position to 0
		return

;
; check motor position and delays
; motor struct is in motorptr
; returns with step position in w
;
;
; check the delay between steps, if it isn't done, then return with
; current step
;
checkmotor
		movlw	MOTCNT
		addwf	motorptr,w	; get offset for count
		movwf	FSR		; set pointer to count
		decf	INDF,f		; decrement motor count
		btfss	STATUS,Z	; count done?
		goto	nostep		; no return current step

;
; The step counter is done, check to see if motor has turned a
; complete revolution. if not, reset the delay between steps
; and bump step to next position
newstep					; yes, check cycle and rot reset delay
		movlw	STPCNT		; move pointer to stepcount
		addwf	motorptr,w
		movwf	FSR
		decf	INDF,f		; decrment step count
		btfss	STATUS,Z	; a cycle complete?
		goto	dostep		; no. do step now

;
; one cycle has completed, check to see if the number of rotations
; require has completed. if not, then reset the cycle count 
; the delay betweens steps, and bump the stepper to the next position
;
		movlw	CYCLECNT	; yes, get the cycle count
		movwf	INDF		; reset the step count
		movlw	ROTCNT		; set pointer to ROTCNT
		addwf	motorptr,w
		movwf	FSR		; set pointer to ROTCNT
		decf	INDF,f		; decrement the ROTCNT
		btfss	STATUS,Z	; see if have done a rot cycle
		goto	dostep		; no, do step with current delay

;
; the required number of rotations has completed. Get the next delay
; between steps from the table, reset rotation count 
;
		movlw	NUMROTS		; yes get the number of rots
		movwf	INDF		; reset the rot count		

;
; get the next delay from the rotation table
;
;		movlw	MOTDELIDX
;		addwf	motorptr,w	; 
;		movwf	FSR		; set pointer to delay index
;		movf	INDF,w		; get the delay index
;		movwf	delayreg	; set the delay register for table
		movlw	MOTDEL		; set the pointer to delay count now
		addwf	motorptr,w	; so later we won't need w to set ptr
		movwf	FSR		; set pointer to delay count
		call	getdelay
		movwf	INDF		; set the new delay count
;		movlw	MOTDELIDX
;		addwf	motorptr,w
;		movwf	FSR		; move pointer to delay index
;		movf	delayreg,w	; get the new delay index
;		movwf	INDF		; set the new delay index
;
; get the delay between steps (either changed or not)
; then do a step
;
dostep					; dostep with current delay
		
		movlw	MOTDEL
		addwf	motorptr,w
		movwf	FSR		; set pointer to motor delay
		movf	INDF,w		; get the current delay
		incf	FSR,f		; bump point to STPCNT
		movwf	INDF		; reset the delay count
		movlw	MOTSTP
		addwf	motorptr,w	; move pointer to step
		movwf	FSR		; step pointter to step
		incf	INDF,f		; bump to next step
		movf	INDF,w		; get the step value
		call	getstep		; put output value in w
		return			; return with step in w

nostep
		movlw	MOTSTP		; no get offset for current step
		addwf	motorptr,w	; set w to pointer value
		movwf	FSR		; set pointer current step
		movf	INDF,w		; get the step
		call	getstep		; get the step value
		return			; return with step value in w



getstep
		andlw	0x7		; free running count, use 3 bits
		addwf	PCL,f		; add cntr to pc value
		retlw	0x09
		retlw	0x01	
		retlw	0x03	
		retlw	0x02
		retlw	0x06
		retlw	0x04
		retlw	0x0C
		retlw	0x08

;
; get the motor delay value by using the
; "random" input from 4 oscillators
;
getdelay
		movlw	0xff
		xorwf	xorm,f		; switch the xor mask
		movf	PORTA,w		; get the current value of 4 osc
		xorwf	xorm,w		; swap bits or not
		andlw	0x0f		; mask off top bits
		addwf	PCL,f		; add to pcl for jump table
		retlw	0x4
		retlw	0x8
		retlw 	0xC
		retlw	0x10
		retlw	0x14
		retlw	0x18
		retlw	0x1C
		retlw	0x20
		retlw	0x24
		retlw	0x28
		retlw	0x2C
		retlw	0x30
		retlw	0x34
		retlw	0x38
		retlw	0x3C
		retlw	0x40
		retlw	0x44
		
		END                     ; directive 'end of program'


;
; get the motor delay value
; delayreg contains the current delay index
; motorptr has the pointer to the motor structure
; return delay value in w
; and he new delay index in delayreg
;
getdelay
		movf	motorptr,w
		sublw	motor1		; is this motor 1 
		btfss	STATUS,Z	
		goto	getmotd2	
		call	getmotdel1
motret
		incf	delayreg,f	; increment the delay index
		andlw	0xff		; test for zero
		btfss	STATUS,Z
		return			; return with value in w
		clrf	delayreg	; clear the delay index
		goto	getdelay	; 

getmotd2
		call	getmotdel2
		goto	motret
		
		
getmotdel1
		movf	delayreg,w
		addwf	PCL,f
		retlw	0x10
		retlw	0x20
		retlw	0x30	
		retlw	0x40
		retlw	0

getmotdel2
		movf	delayreg,w
		addwf	PCL,f
		retlw	0x40
		retlw	0x30
		retlw	0x10	
		retlw	0	

