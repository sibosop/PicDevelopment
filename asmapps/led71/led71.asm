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
		movlw	0x90		; enable port b interrupts
		movwf	INTCON		; store value in interrupt control
		bsf	STATUS,5	; select page 1
		movlw	0x01
		movwf	PORTB		; set 0 - input, rest output
		bcf	STATUS,5	; select page 0

		clrf	cntr		; clear switch counter
		call	table		; send 0 to leds
		movwf	led_store	; save w
		rlf	led_store,w	; rotate into port bits
		movwf	PORTB

loop
		movf	cntr,w		; get counter into w
		andlw	0xf		; get low nibble
		call	table		; get table value
		movwf	led_store	; get led value
		rlf	led_store,w	; shift left, store in W
		movwf	PORTB		; output value to port
		goto	loop		; wait for switch

table
		addwf	PCL,f		; add cntr to pc value
		retlw	0x3f
		retlw	0x06
		retlw	0x5b
		retlw	0x4f
		retlw	0x66
		retlw	0x6d	
		retlw	0x7d	
		retlw	0x07
		retlw	0x7f
		retlw	0x6f
		retlw	0x77
		retlw	0x7c
		retlw	0x39
		retlw	0x5e
		retlw	0x79
		retlw	0x71
		
	
	
	

		END                     ; directive 'end of program'



