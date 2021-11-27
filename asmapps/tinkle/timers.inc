#ifndef TIMERS_INC
#define TIMERS_INC

#ifndef IN_TIMERS
	extern inittimers
	extern checktimers
	extern timers
	extern timermask
	extern tickcheck
	extern	getlowticks
	extern	gethighticks
	extern	tempoCnt
	extern	ticker
	extern	tempoRem
	extern	remtick
#endif
	constant	numbeatticks = D'240'
	constant	minmillisecs = D'60000'
settempo macro	tempo
	movlw	(minmillisecs/(tempo * numbeatticks))
	banksel	tempoCnt
	movwf	tempoCnt
	local	remain = (minmillisecs%(tempo * numbeatticks))
	movlw	(((tempo * numbeatticks) / remain))
	movwf	tempoRem
	movf	tempoCnt,w
	movwf	ticker
	movf	tempoRem,w
	movwf	remtick
	endm

	constant thigh	= 1
	constant tlow	= 0
	constant numbeatticks = D'240'

; emun of wait values
	constant _whole	 	= D'0'

	constant _half	 	= D'1'
	constant _half3 		= D'2'
	constant _half5 	 	= D'3'

	constant _quart	 	= D'4'
	constant _quart3  	= D'5'
	constant _quart5  	= D'6'

	constant _8th	 	= D'7'
	constant _8th3   	= D'8'
	constant _8th5   	= D'9'

	constant _16th	 	= D'10'
	constant _16th3   	= D'11'

	constant _32th	 	= D'12'
	constant _32th3   	= D'13'

	constant _64th	 	= D'14'
	constant _64th3   	= D'15'

; tick wait value constants
	constant __whole	= (numbeatticks * 4)

	constant __half	= (__whole/2)
	constant __half3	= (__whole/3)
	constant __half5 	= (__whole/5)

	constant __quart	= (__half/2)
	constant __quart3 = (__half/3)
	constant __quart5 = (__half/5)

	constant __8th	= (__quart/2)
	constant __8th3  = (__quart/3)
	constant __8th5  = (__quart/5)

	constant __16th	= (__8th/2)
	constant __16th3  = (__8th3/3)

	constant __32th	= (__16th/2)
	constant __32th3  = (__16th3/3)

	constant __64th	= (__32th/2)
	constant __64th3  = (__32th3/3)

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


	
#endif
	
