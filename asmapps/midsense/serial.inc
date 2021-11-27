#ifndef SERIAL_INC
#define SERIAL_INC

; serial.inc
; header for UART support code for 16C73B PIC
;
; define global values, etal

; baudrate values for SPBRG register at 20Mhz with BRGH=0 (slow speed)
; Baud rate calculation is (FOSC/(BAUDR * 64)) - 1 rounded off
;
	if	(clockrate == 20)
BAUD24		equ	D'129'	; 2400
BAUD96		equ	D'32'	; 9600
BAUD192		equ	D'15'	; 19200
BAUDMIDI	equ	D'9'	; 31250 for midi
	endif

	if	(clockrate == 10)
BAUDMIDI	equ	D'4'	; 31250 for midi
	endif

	if	(clockrate == 4)
BAUDMIDI	equ	D'1'	; 31250 for midi
	endif



; external symbols for users
	ifndef UARTSUPP
	extern uartinit 
	extern uartwrite
	extern chkutrans
	extern chkurecv
	endif
#endif
