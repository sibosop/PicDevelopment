; main.asm
; Main routine for ROBOCAR
;  (c) M.Schippling Y2k+1
;
; sub-routine entry point: init
; called from startup.asm on reset
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#include	serial.inc

; define global registers
	UDATA
IOSTAT		res	1	; program status bits
	global IOSTAT

; define local registers, etal
dtemp0		res	1	; delay loop and general temporaries
dtemp		res	1


; Put it in ROM PAGE0
PROG1	CODE
; Initialization entry point, called from startup
init
	global init

	; clear necessary registers
	clrf	IOSTAT

	; initialize read and write buffers just for fun
	call ireaddata
	call iwritedata

	; setup UART to 9600 baud
	movlw	BAUD96
	call	uartinit

	; enable interrupts
	bsf	INTCON,PEIE	; peripheral ints
	bsf	INTCON,GIE	; everybody

	; fire it off once first
	call	ustartwrite

; The MAIN loop thing
main
	; read data, do stuff, then fill WRITE comm buffer to UART

	;check read ready status
	btfss	IOSTAT, UARTRD		; wait until read buffer is ours
	goto	skipwrite		; not ready yet

	;check write ready status
	btfss	IOSTAT, UARTWR		; wait until write buffer is ours
	goto	skipwrite		; not ready yet

	; fill the write buffer
	call	setdata

	; fire it off
	call	ustartwrite		; adds SYNCPAT to head of stream

skipwrite
	; do it again daddy!!
	movlw	0x01
	call longdelay
	goto	main		


; delay	-- approx 200 microsec
; longdelay -- W reg X 200 microsec
;
;		delay loops -- use dtemp0 and dtemp registers
;
delay
	movlw	0xff
	movwf	dtemp0
d0	nop
	decfsz	dtemp0,f
	goto	d0
	return

longdelay
	movwf	dtemp
d1	call	delay
	decfsz	dtemp,f
	goto	d1
	return


; setdata -- sub-routine to collect useful information into WRITE buffer
;		copies read buffer into write buffer when its ready
;
setdata
	;check read ready status
	btfss	IOSTAT, UARTRD		; if buffer is not ours
	return				; just return

	; otherwise copy read to write
	movlw	UWRBUF
	movwf	FSR
copyread	; bruteforce copy readbuf to write buf
	movf	URDBUF,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+1,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+2,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+3,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+4,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+5,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+6,w
	movwf	INDF
	incf	FSR,f
	movf	URDBUF+7,w

	; reset read buf
	bcf	IOSTAT, UARTRD		;   set it back to theirs

	return

; initialize data in write buffer
iwritedata
	movlw	UWRBUF
	movwf	FSR
	movlw	0x41	; start with 'A'
	movwf	dtemp

_loadbuf	MACRO
	movwf	INDF
	incf	FSR,f
	incf	dtemp,f
	movf	dtemp,w
		ENDM

	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	movlw	'\n'	; end with \n
	movwf	INDF
	return

; ireaddata -- sub-routine to init information in read buffer
;
ireaddata
	movlw	URDBUF
	movwf	FSR
	movlw	0x61	; start with 'a'
	movwf	dtemp

	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	_loadbuf
	movlw	'\n'	; end with \n
	movwf	INDF
	return

	END
