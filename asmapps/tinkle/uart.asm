; uart.asm

; sub-routine entry point: inituart
;			   writeuart
; called from startup.asm with base registers saved
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#define		UARTSUPP
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include	tinkle.inc

; define local registers, etal

RAM1	UDATA	
writereg	res	1
writebuf	res	queue_size

; Put it in ROM PAGE0
PROG1	CODE

;
; initialize the uart
uartinit
	global uartinit
	initqueue writebuf
	banksel	SPBRG
	movlw 	BAUDMIDI	; get the midi baud rate	
	movwf	SPBRG		; set the baud rate register (low speed)
	banksel	TXSTA
	bcf	TXSTA,SYNC	; set to asynchronous mode
	banksel	RCSTA
	bsf	RCSTA,SPEN	; serial port enable bit
	banksel	PIE1
	bsf	PIE1,TXIE	; enable transmit interrupt
	banksel	TXSTA
	bsf	TXSTA,TXEN	; Enable transmission
	return

;
; uart transmit code
;

; see if transmitter is ready for sending a character
chkutrans
	global	chkutrans
	banksel	PIR1		
	btfss	PIR1,TXIF	; test transmit empty
	return			; return if transmit not ready
	dequeue	writebuf	; get the next byte to transmit
	btfss	STATUS,Z	; see if queue is empty
	goto	putchar		; no, send a character (in w)
	banksel	PIE1		; yes, queue is empty
	bcf	PIE1,TXIE	; disable transmit interrupt
	return			; 
putchar
	banksel	TXREG		;
	movwf	TXREG		; stuff w into transmit register
	banksel	PIE1
	bsf	PIE1,TXIE	; enable transmit empty interrupt
	goto	chkutrans	; check again, shift register was
				; empty, then two characters can
				; be written

;
; queue a character up for writing to the uart
;
uartwrite
	global uartwrite
	banksel	writereg
	movwf	writereg	; save write value (enqueue wacks w)
	enqueue	writebuf,writereg,0	; enqueue the write value
	banksel	PIE1
	bsf	PIE1,TXIE
	return


;
; uart receive code
;
chkurecv
	global chkurecv
	return

	END
