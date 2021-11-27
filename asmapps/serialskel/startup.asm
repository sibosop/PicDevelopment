; startup.asm
; Startup code for 16C73B PIC
;   executes symbol "init" in ROM page 0 on reset
;   calls symbol "inter" in ROM page 0 on interrupt
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include 	"..\shared\macros.inc"

; Set config bits from defines in .INC file
; Config address for device programmer is in 16F84A.lkr
;			xtal + pwrup reset ON + wdog timer OFF + code prot OFF
	__CONFIG	_XT_OSC & _PWRTE_ON & _WDT_OFF & _CP_OFF       

; External symbols that need to be in other modules
	EXTERN init, inter

;these are the local registers for the interrupt to save status
		UDATA_SHR
w_temp		res	1       ; W -- needs to be in all RAM banks
				;  the rest are in bank 0
gpr0		UDATA
status_temp	res	1	; STATUS -- gotta have it
fsr_temp	res	1	; FSR -- indirect addressing vector
pclath_temp	res	1	; PCLATH -- rom bank


; vectors to executable code
STARTUP	CODE
_reset	goto	init

			; NOTES: assumes RAM bank 0, needs w_temp in all banks
			;	 assumes ROM page 0 for all code
INTR	CODE
_inter	movwf	w_temp		; save contents of W register
	swapf	STATUS,w	; move status register into W register
	bcf	STATUS,RP0	; set RAM bank 0
	disable
	movwf	status_temp	; save old contents of STATUS
	movf	FSR,w		; save contents of FSR 
	movwf	fsr_temp
	movf	PCLATH,w	; save contents of PCLATH
	movwf	pclath_temp
	call	inter		; Call the real interrupt code
	movf	pclath_temp,w	; restore contents of PCLATH
	movwf	PCLATH
	movf	fsr_temp,w	; restore contents of FSR
	movwf	FSR
	swapf	status_temp,w	; fix copy of STATUS
	movwf	STATUS          ; restore STATUS register contents
	swapf	w_temp,f	; SWAP doesn't affect STATUS register!!
	swapf	w_temp,w	; restore W register contents
	enable
	retfie			; return from interrupt

	END

; ROM ID location (arbitrary value) 
IDLOCS	CODE
dw 0x0102
dw 0x0304

