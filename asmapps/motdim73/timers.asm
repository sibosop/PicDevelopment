
; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_TIMERS
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	minput.inc
	#include 	timers.inc
	#include 	math.inc

RAM2	UDATA	

timers		res	16	; 2 bytes for each timer
timermask	res	1

	global	timers
	global	timermask

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
	
	

	END
