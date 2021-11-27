; uartsupp.asm
; UART support code for 16C73B PIC
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#include	P16C73B.INC
	#define		UARTSUPP	; to turn off externs in serial.inc
	#include	serial.inc

; define global registers
	UDATA
URDPTR		res	1	; pointer to next read buffer byte
UWRPTR		res	1	; pointer to next write buffer byte
utemp		res	1	; temp for interrupt routine use

	global URDPTR, UWRPTR
	extern IOSTAT		; defined in main

; define local registers and values
URDEND		equ	URDBUF + URDLEN	; one past last byte of read buffer
UWREND		equ	UWRBUF + UWRLEN	; one past last byte of write buffer

	CODE

; uartinit --  UART initialization sub-routine
;		call with baudrate defined in W register
;		setup async 8-bit uart
;		both RC and TX interrupts are enabled
;		returns with RAM Bank0 set
;
uartinit
	global	uartinit
	clrf	URDPTR		; clear special registers
	clrf	UWRPTR
	bcf	IOSTAT, UARTRD	; read buffer belongs to interrupt
	bsf	IOSTAT, UARTWR	; write buffer belongs to main
	BANKSEL	SPBRG		; set RAM Bank 1
	movwf	SPBRG		; Set Baud rate from W
	movlw	0x00		; 8-bit transmit, transmitter disabled,
	movwf	TXSTA		; asynchronous mode, low speed mode
	bcf	TRISC, 6	; set TX port to output (superflous?)
	bsf	PIE1,RCIE	; Enable receive interrupts
	BANKSEL	RCSTA		; set RAM Bank 0
	movlw	0x90		; 8-bit receive, receiver enabled,
	movwf	RCSTA		; serial port enabled
	return


; uintread -- UART interrupt read sub-routine
;		reads URDLEN bytes of packet into URDBUF buffer until filled
;		checks and assumes that packet starts with SYNCPAT value
;		no overrun or error handling
;		assumes RAM bank0 on entry and return
;
;	The main program should do this:
;		if( IOSTAT->UARTRD == UBINT )
;			return err;
;		else
;			deal with read data;
;			call ustartread
;
uintread
	global uintread

    ; macro to read a byte from uart and put it at the right place in the buffer
    ;  assumes that the URDPTR is in the W register to start with
    ;  increment buf pointer and return the byte read in W
uartrdbyte	MACRO	num	; arbitrary number to use for local labels
	;  check FERR and OERR bits and reset if error
	btfsc	RCSTA, FERR
	goto	_urderror
	btfsc	RCSTA, OERR
	goto	_urderror
	;  load read pointer into FSR
	movwf	FSR
	;  read UART data into W
	movf	RCREG,w
	;  indirect store of W to buffer at read pointer
	movwf	INDF
	movwf	utemp		; save for later too
	;  increment read pointer in place
	incf	URDPTR,f
	;  if pointer is at end --
	;	set IOSTAT->UARTRD = UBMAIN;
	;	set read pointer   = null;
	movlw	URDEND
	subwf	URDPTR,w	; if PTR == END we get zero here
	btfss	STATUS, Z	; if non-zero
	goto	_urdmout#v(num)	;  just retrieve value
	movwf	URDPTR		; otherwise reset buffer stuff (0 in W now)
	bsf	IOSTAT, UARTRD	;  and pass buffer control to main
	;  return with read data in W so we can check it
_urdmout#v(num)
	movf	utemp,w
		ENDM

	; if not ready to read, we need to cleanup and bail
	;	so reset interrupt and ignore data
	btfsc	IOSTAT, UARTRD
	goto	_urderror	; we don't own the buffer

	; if read pointer is not set, we are starting a new packet
	;	so set read pointer and assume the end is where we think
	movf	URDPTR,w
	btfss	STATUS, Z
	goto	_ureadbyte	; just read byte if PTR is not Zero
	movlw	URDBUF		;  otherwise if it was zero,
	movwf	URDPTR		;  setup and save PTR 
	uartrdbyte 0		; then read the first byte to buf and W reg

	; if we didn't get the starting SYNCPAT value
	;	reset interrupt and read pointer to ignore data
	; else data was read and interrupt reset, just return
	xorlw	SYNCPAT		; check pattern
	btfsc	STATUS, Z	; if its zero,
	return			;  its cool
	goto	_urderror	; wasn't zero, treat it like an error
	
	; else if read pointer was set we are getting more data
	;	so do uartrdbyte
_ureadbyte		; just read the next byte
	uartrdbyte 1

	; interrupt was reset by RCREG read, just return
	return
    
_urderror	; we don't own the buffer or other error, lets reset and bail
	bcf	RCSTA,CREN	; clear possible error
	movf	RCREG,w		; get garbage a couple times
	movf	RCREG,w		; get garbage a couple times
	bsf	RCSTA,CREN	; reset receive
	clrf	URDPTR		; clear out the pointer to start over
	return


; ustartread -- subroutine to fire off a buffer read.
;		clear read pointer
;		tell interrupt to take over
;
ustartread
	global	ustartread
	clrf	URDPTR		; setup read buffer pointer for start
	bcf	IOSTAT, UARTRD	; mark interrupt as owner of buffer
	return


; uintwrite -- UART interrupt write sub-routine
;		writes UWRLEN bytes of packet from UWRBUF buffer until done
;		packet will be prefaced with SYNCPAT value by main routine
;			(so first SYNCPAT byte in NOT in buffer)
;		no overrun or error handling
;		assumes RAM bank0 on entry and return
;
;	The last interrupt (after the last byte from BUF is sent)
;	 resets the TXEN and TXIE bits to disable transmission until
;	 ustartwrite is called again...
;
;	The main program should do this:
;		if( IOSTAT->UARTWR == UBINT )
;			return not ready;
;		else
;			setup write buffer data;
;			call ustartwrite	-- sends SYNCPAT and starts ints
;
uintwrite
	global uintwrite

    ; macro to write a byte to the UART from the UWRPTR location
    ;  assumes that the UWRPTR is in the W register to start with
    ;  increment buf pointer and clear it out when it reaches the end
uartwrbyte	MACRO	num	; arbitrary number to use for local labels

	;  load write pointer into FSR
	movwf	FSR
	;  indirect get of buffer at write pointer into W
	movf	INDF,w
	; send data to UART
	movwf	TXREG

	; This seems to be necessary for each byte???
	BANKSEL	SPBRG		; set RAM Bank 1
	bsf	TXSTA,TXEN	; Enable transmission
	BANKSEL	RCSTA		; set RAM Bank 0

	;  increment write pointer in place
	incf	UWRPTR,f

	;  if pointer is at end --
	;	set IOSTAT->UARTWR = UBMAIN;
	;	set write pointer   = null;
	movlw	UWREND
	subwf	UWRPTR,w	; if PTR == END we get zero here
	btfss	STATUS, Z	; if non-zero we've go more to write
	goto	_uwrmout#v(num)	;  otherwise just leave
	movwf	UWRPTR		; otherwise reset buffer stuff (W is Zero now)
	bsf	IOSTAT, UARTWR	;  and pass buffer control to main

_uwrmout#v(num)
		ENDM

	

	; if not ready to write, clear interrupt enable and return
	btfsc	IOSTAT, UARTWR
	goto	_uwrdone	; we don't own the buffer

	; otherwise write the next byte
	movf	UWRPTR,w	; put the pointer in W
	uartwrbyte 0		; write a byte

	; interrupt was reset, now return
	return

_uwrdone	; we're all done with writing for now, error or not
	clrf	UWRPTR		; clear PTR just in case
	BANKSEL	SPBRG		; set RAM Bank 1
	bcf	PIE1,TXIE	; Disable transmit interrupts
	bcf	TXSTA,TXEN	; Disable transmission
	BANKSEL	RCSTA		; set RAM Bank 0
	return


; ustartwrite -- subroutine to fire off a buffer write.
;		tell interrupt to take over
;		write SYNCPAT byte to UART
;		enable TXIE interrupts
;		  note -- interrupt and next byte will be loaded immediately
;
ustartwrite
	global	ustartwrite
	movlw	UWRBUF		; setup write buffer pointer
	movwf	UWRPTR
	bcf	IOSTAT, UARTWR	; mark interrupt as owner of buffer
	movlw	SYNCPAT		; write SYNCPAT byte to UART
	movwf	TXREG
	; enable write interrupts
	BANKSEL	SPBRG		; set RAM Bank 1
	bsf	PIE1,TXIE	; Enable transmit interrupts
	bsf	TXSTA,TXEN	; Enable transmission
	BANKSEL	RCSTA		; set RAM Bank 0
	return

	END
