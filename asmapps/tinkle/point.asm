
; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_POINT
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include	tinkle.inc
	#include	upstruct.inc
	#include	midi.inc
	#include	math.inc
	#include	timers.inc
	#include 	phrases.inc
	#include 	point.inc

OVR1	UDATA_OVR
	constant	tempo	= D'60'
	constant	ltimer = 0
	constant	rtimer = 1
	constant	rctimer = 2
	constant	botimer = 3

	constant	rightChords = 0
	constant	rightWait   = 1
	constant	rightReps   = 2
	constant	rightTrems  = 3
	
	constant	down = 0
	constant	up = 1

	constant	leftBassNote    = 0
	constant	bassOn	    = 1

	constant	rchordstart = D'58'
	constant	rchordend   = D'66'

	constant	replow	= D'78'
	constant	rephigh = D'88'


pointlstate	res	1
pointrstate	res	1


rightchordvel	res	1
rtmp		res	1
rcnt		res	1
repcnt		res	1
repstate	res	1

rightchord	res	4

leftChord	res	4

leftBass	res	1
leftBassCnt	res	1
leftBassVel	res	1

PROG1	CODE
pointInit	
	global pointInit
	banksel	pointlstate
	settempo tempo
	clrf	pointrstate
	clrf	pointlstate
	clrf	rightchordvel
	clrf	leftBassCnt
	clrf	repcnt
	clrf	repstate
	movlw	1
	movwf	rcnt
	bsf	pointrstate,rightChords
	bsf	pointlstate,leftBassNote
	return

checkPoint
	global	checkPoint
	banksel	timermask
	btfss	timermask,ltimer
	call	doLeft
	btfss	timermask,rtimer
	call	doRight
	btfss	timermask,rctimer
	call	rchordoff
	btfss	timermask,botimer
	call	bOff
	return

doLeft	
	btfsc	pointlstate,leftBassNote
	goto	doLeftBassNote
	return


doLeftBassNote
	incf	leftBassCnt,f
	btfss	leftBassCnt,0
	goto	secondBass
	randnum	D'36',D'45',1
	movwf	leftBass
	randnum	D'20',D'30',1
	movwf	leftBassVel
	midpnl	leftBass,0,leftBassVel,0
	banksel	leftBass
setbtimer
	movlw	_whole
	call	getlowticks
	movwf	rtmp
	call	getrand
	banksel	rtmp
	andlw	7
	addwf	rtmp,f
	stimf	ltimer,rtmp,0,tlow
	movlw	_whole
	call	gethighticks
	movwf	rtmp
	call	getrand
	banksel rtmp
	andlw	3
	addwf	rtmp,f
	stimf	ltimer,rtmp,0,thigh
	return

secondBass
	midpnl	leftBass,0,0,1
	banksel	leftBass
	randnum	0,10,1
	btfsc	STATUS,Z
	goto	setbtimer
	randnum	D'46',D'52',1
	movwf	leftBass
	randnum	D'5',D'10',1
	movwf	leftBassVel
	midpnl	leftBass,0,leftBassVel,0
	banksel	tmp1
	randnum	__16th,__8th,1
	movwf	tmp1
	stimf	botimer,tmp1,0,tlow
	bsf	pointlstate,bassOn
	goto	setbtimer

bOff
	btfss	pointlstate,bassOn
	return
	bcf	pointlstate,bassOn
	midpnl	leftBass,0,0,1
	return
	
	

	






doRight
	decf	rcnt,f
	btfss	STATUS,Z
	goto	doR1
	clrf	pointrstate
	bsf	pointrstate,0
	randnum	D'3',D'10',1
	movwf	rcnt
	randnum	D'0',D'3',1
	movwf	rtmp
doR2
	btfsc	STATUS,Z
	goto	doR1
	slf	pointrstate,f
	decf	rtmp,f
	goto	doR2
doR1
	btfsc	pointrstate,rightChords
	goto	doRightChords
	btfsc	pointrstate,rightWait
	goto	doRightWait
	btfsc	pointrstate,rightReps
	goto	doRightReps
	btfsc	pointrstate,rightTrems
	goto	doRightTrems
	return


rchordoff
	movf	repstate,f	
	btfss	STATUS,Z
	return
	movf	rightchordvel,f
	btfsc	STATUS,Z
	return
	clrf	rightchordvel
doChord
	movf	rightchord,w
	call	putn
	movf	rightchord+1,w
	call	putn
	movf	rightchord+2,w
	call	putn
	movf	rightchord+3,w
	call	putn
	return



doRightChords
	randnum	rchordstart,rchordend,1
srcl
	movwf	rightchord
	addlw	6
	movwf	rightchord+1
	addlw	5
	movwf	rightchord+2
	randnum	3,5,1
	addwf	rightchord,w
	movwf	rightchord+3
setcvel	
	randnum	D'5',D'20',1
	movwf	rightchordvel
	call	doChord
	call	doRightWait
	randnum	_whole,_16th,1
	movwf	rtmp
	call	getlowticks
	movwf	tmp1
	stimf	rctimer,tmp1,0,tlow
	movf	rtmp,w
	call	gethighticks
	movwf	tmp1
	stimf	rctimer,tmp1,0,thigh
	return

putn
	movwf	rtmp
	midpnl	rtmp,0,rightchordvel,0
	banksel	rtmp
	return
	
	
doRightWait
	movlw	_whole
	call	getlowticks
	movwf	rtmp
	call	getrand
	banksel	rtmp
	andlw	7
	addwf	rtmp,f
	stimf	rtimer,rtmp,0,tlow
	movlw	_whole
	call	gethighticks
	movwf	rtmp
	call	getrand
	banksel rtmp
	andlw	3
	addwf	rtmp,f
	stimf	rtimer,rtmp,0,thigh
	return

	


doRightReps
	movf	repstate,f
	btfss	STATUS,Z
	goto	checkrep
	bsf	repstate,up
	movlw	D'10'
	movwf	rightchordvel
	movwf	repcnt
	randnum	replow,rephigh,1
	movwf	rightchord
checkrep
	decf	repcnt,f
	btfss	STATUS,Z
	goto	rep1
	btfss	repstate,down
	goto	setdown
	clrf	repstate
	movlw	1
	movwf	rcnt
	return
setdown
	bcf	repstate,up
	bsf	repstate,down
	movlw	D'10'
	movwf	repcnt
rep1	
	midpnl	rightchord,0,0,1
	btfss	repstate,up
	goto	repdown
	movlw	D'10'
	addwf	rightchordvel,f
	goto	repnote
repdown
	movlw	D'10'
	subwf	rightchordvel,f
repnote
	movf	rightchord,w
	call	putn
repwait
	movlw	__16th
	movwf	rtmp
	stimf	rtimer,rtmp,0,tlow
	movlw	2
	movwf	rcnt
	return


doRightTrems
	return



	END
