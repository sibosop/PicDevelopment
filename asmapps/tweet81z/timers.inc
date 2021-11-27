#ifndef TIMERS_INC
#define TIMERS_INC

#ifndef IN_TIMERS
	extern inittimers
	extern checktimers
	extern timers
	extern timermask
#endif
	constant thigh	= 1
	constant tlow	= 0
;
; set entire timer word using literal time
;
stimw	macro	num, time
	banksel	timers
	movlw	(time & 0xff)
	movwf	(timers+(num*2))
	movlw	(time / 0x100)
	movwf	(timers+(1+(num*2)))
	bsf	timermask,num
	endm

;
; this macro expects that the low timer
; will be set first. setting the low
; timer clears the high timer so that
; both don't always have to be set
;
stimf	macro	num,time,littime,hflag
	banksel	timers
	movlw   (timers+(hflag+(num*2)))
	bsf	timermask,num
	movwf	FSR
	if ( littime == 1 )
	movlw	time
	else
	banksel time
	movf	time,w
	endif
	banksel	timers
	movwf	INDF
	if ( hflag == tlow )	
	incf	FSR,f
	clrf	INDF
	endif
	endm

stimfn	macro	num,mask,time,hflag
	banksel mask
	movf	mask,w
	banksel	timermask
	iorwf	timermask,f
	banksel	num
	movf	num,w
	bcf	STATUS,C
	addwf	num,w		; w = num * 2
	addlw	hflag		; w += hflag (high byte if one
	banksel	timers
	addlw	timers		; make pointer offset from times
	movwf	FSR		; store point
	banksel	time
	movf	time,w
	banksel	timers
	movwf	INDF
	endm


	
#endif
	
