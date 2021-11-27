
; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_TIMERS
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	midsense.inc
	#include 	timers.inc
	#include 	math.inc

RAM2	UDATA	

timers		res	16	; 2 bytes for each timer
timermask	res	1

#ifdef USE_KHZ
tempoCnt	res	1
ticker		res	1
tempoRem	res	1
remtick		res	1
#endif
	global	timers
	global	timermask
#ifdef USE_KHZ
	global	tempoCnt
	global	ticker
	global	tempoRem
	global	remtick
#endif

	extern tmp1
	extern tmp2
	extern tmp3
	extern tmp4


PROG1	CODE


inittimers	
	global	inittimers
	banksel	tmp1
	movlw	1			; set the timer mask to timer 0
	movwf	tmp1			
	movlw	timers			; get the timer array base
	movwf	FSR			; set indirection pointer
initloop
	clrf	timermask		; clear all timerflags
	clrf	INDF			; clear timerlo
	incf	FSR,f			; bump to timer hi
	clrf	INDF			; clear timer hi
	incf	FSR,f			; bump to next timer
	slf	tmp1,f			; shift mask to next timer
	btfss	STATUS,C		; bit goes into carry when done
	goto	initloop		; not done, do next timer
	return				; done

#ifdef USE_KHZ
tickcheck
	global	tickcheck
	banksel	tempoCnt
	decf	ticker,f
	btfss	STATUS,Z
	return
	movf	tempoCnt,w	; get the normal tempo division
	movwf	ticker		; store in the ticker
	movf	tempoRem,w	; get the remainer ticker
	btfsc	STATUS,Z	; is it zero
	goto	done		; yes, then just return
	decf	remtick,f	; test the remtick
	btfss	STATUS,Z	; if zero then we need to bump the ticker
	goto	done		; not time to bump ticker
	incf	ticker,f	; bump the ticker
	movf	tempoRem,w
	movwf	remtick
done
	bsf	STATUS,Z	; it's time man
	return			; 
#endif
	
	
	
	


checktimers
	global	checktimers
	banksel	tmp1			
	movlw	timers			; get base of timer array
	movwf	tmp2			; set timer pointer
	movlw	1			; set mask to timer 0
	movwf	tmp1			; set test mask
checkloop
	movf	tmp2,w			; get current timer ptr
	movwf	FSR			; store in indirection ptr
	movf	tmp1,w			; get the test mask
	andwf	timermask,w		; see if current timer is set
	btfsc	STATUS,Z		; zero if timer not set
	goto	nexttimer		; zero? try next timer
	movlw	1			; for decrement
	subwf	INDF,f			; decrement timer lo
	btfsc	STATUS,C		; test c for overflo
	goto	nexttimer		; C set then, no overflow
	incf	FSR,F			; bump to timer hi (doesn't effect C)
	subwf	INDF,f			; decrement timer hi
	btfsc	STATUS,C		; test c for overflow
	goto	nexttimer		; C set then, no overflow
	comf	tmp1,w			; complement the test mask
	andwf	timermask,f		; clears current timer bit
nexttimer
	movlw	2			; timers are double bytes
	addwf	tmp2,f			; bump ptr to next timer
	slf	tmp1,f			; shift mask to next timer
	btfss	STATUS,C		; mask went into carry 
	goto	checkloop		; no, then do more
	return				; yes, then quit
	
	
JUMPS7	CODE
	table	getlowticks
	retlw low __whole	
	retlw low __half	
	retlw low __half3	
	retlw low __half5 	
	retlw low __quart	
	retlw low __quart3 
	retlw low __quart5 
	retlw low __8th	
	retlw low __8th3  
	retlw low __8th5  
	retlw low __16th	
	retlw low __16th3  
	retlw low __32th	
	retlw low __32th3  
	retlw low __64th	
	retlw low __64th

	table	gethighticks
	retlw high __whole	
	retlw high __half	
	retlw high __half3	
	retlw high __half5 	
	retlw high __quart	
	retlw high __quart3 
	retlw high __quart5 
	retlw high __8th	
	retlw high __8th3  
	retlw high __8th5  
	retlw high __16th	
	retlw high __16th3  
	retlw high __32th	
	retlw high __32th3  
	retlw high __64th	
	retlw high __64th


	global	getlowticks
	global 	gethighticks

	END
