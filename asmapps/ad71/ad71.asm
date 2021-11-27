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
w_temp        	RES     1           ; variable used for context saving 
status_temp   	RES     1           ; variable used for context saving
cntr		RES	1	  ; switch counter




STARTUP CODE
;**********************************************************************
reset		goto    main             ; go to beginning of program
		nop			; fill space till interrupt
		nop			; vector
		nop
interrupt
		nop
PROG	CODE
main
; initialization code
		bsf	STATUS,5	; page 1
		movlw	0
		movwf	INTCON		; disable all interrupts
		movwf	PORTB		; make all ouputs
		movwf	ADCON0		; make all ad input
		bcf	STATUS,5	; page 0
		movlw	0xc1		; RC Clock/ad ch 0
		movwf	ADCON0
loop
		bsf	ADCON0,GO_DONE	; start first conversion
test
		btfsc	ADCON0,GO_DONE
		goto	test
		movf	ADRES,w
		movwf	PORTB
		goto	loop
		
		
		
		
	
	
	

		END                     ; directive 'end of program'



