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
;w_temp        	RES     1           ; variable used for context saving 
;status_temp   	RES     1           ; variable used for context saving
cntr		RES	1	  ; switch counter
led_store	RES	1




STARTUP CODE
;**********************************************************************
reset		goto    main             ; go to beginning of program
		nop			; fill space till interrupt
		nop			; vector
		nop

interrupt   	
		bcf	INTCON,1	; clear interrupt flag	
PROG	CODE
		incf	cntr,f		; increment counter
		retfie                  ; return from interrupt


main
; initialization code
;		movlw	0x90		; enable port b interrupts
;		movwf	INTCON		; store value in interrupt control
		bsf	STATUS,5	; select page 1
		clrf	PORTB		; all bits output
		movlw	0x04
		movwf	TMR0		; set timer prescale, select TMR0
		bcf	STATUS,5	; select page 0

		clrf	cntr		; clear counter
		movlw	0x7e		; send 0 to leds
		movwf	PORTB

start
		clrf	TMR0		; clear timer 0
		bcf	STATUS,Z
loop
		movlw	0xff		; test count
		xorwf	TMR0,w		; has it hit ff
		btfss	STATUS,Z	; loop until it does
		goto	loop	
		incf	cntr,f		
		movf	cntr,w		
		andlw	0x07		; eight steps
		call	table		; get table value
		movwf	PORTB		; output value to port
		goto	start		; wait for switch

table
		addwf	PCL,f		; add cntr to pc value
		retlw	0x08
		retlw	0x0C
		retlw	0x04
		retlw	0x06
		retlw	0x02
		retlw	0x03	
		retlw	0x01	
		retlw	0x09
		
	
	
	

		END                     ; directive 'end of program'



