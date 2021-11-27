; serial.inc
; header for UART support code for 16C73B PIC
;
; define global values, etal

; baudrate values for SPBRG register at 20Mhz with BRGH=0 (slow speed)
BAUD24		equ	D'129'	; 2400
BAUD96		equ	D'32'	; 9600
BAUD192		equ	D'15'	; 19200

; IOSTAT register bits and meanings
UARTRD		equ	0	; UART read buffer status bit
				;   0 == interrupt owner (read in progress)
				;   1 == main owner (buffer in use)
UARTWR		equ	1	; UART write buffer status bit
				;   0 == interrupt owner (write in progress)
				;   1 == main owner (buffer in use)

; read and write buffer lengths--make sure it fits with the BUFs below
URDLEN		equ	8	; numbytes in UART read buffer (with SYNCPAT)
UWRLEN		equ	8	; numbytes in UART write buffer (minus SYNCPAT)

; buffers at top of memory
;  because URDEND expr doesn't work w/o real addr
URDBUF		equ	0x70		; beginning of UART read buffer
UWRBUF		equ	URDBUF+URDLEN	; beginning of UART write buffer

; First byte of packet in a misguided attempt to synchronize on errors
;SYNCPAT		equ	0xAA		; sorta unique pattern
SYNCPAT		equ	0x25		; sorta unique pattern

; external symbols for users
	ifndef UARTSUPP
	extern URDPTR, UWRPTR			; data
	extern uartinit, uintread, uintwrite	; code
	extern ustartwrite
	endif
