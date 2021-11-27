	list      P=PIC16c71             ; list directive to define processor
	#include <p16c71.inc>         ; processor specific variable definitions

; CONFIG is not used by the prom programmer
	__CONFIG   _CP_OFF & _WDT_OFF & _RC_OSC 

; '__CONFIG' directive is used to embed configuration data within .asm file.
; The lables following the directive are located in the respective .inc file.
; See respective data sheet for additional information on configuration word.



		extern interrupt
		extern main



STARTUP CODE
;**********************************************************************
reset		goto    main             ; go to beginning of program
		nop			; fill space till interrupt
		nop			; vector
		nop
		goto	interrupt

PROG	CODE
		
		
		END                     ; directive 'end of program'
