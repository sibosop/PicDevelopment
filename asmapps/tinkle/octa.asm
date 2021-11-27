
; define chip to be used and include the standard stuff
	processor	16C73B
	#define		IN_OCTA
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
	#include 	octa.inc

OVR1	UDATA_OVR
	constant	tempo	= D'72'
	constant	beattimer   = 0
	constant	chordtimer  = 1
	constant	melodytimer = 2
	constant	legatotimer = 3
	constant	basstimer   = 4
	constant	numbeats    = 5

	constant	chordlow    = D'46'
	constant	chordhigh   = D'54'
	constant	firstvel    = D'20'
	constant	followvel    = D'10'
	constant	bassvel	 	= D'30'

	constant	melodyvelstart   = D'40'
	constant	melodyvelend	= D'60'
	constant	melodyvellast	= D'10'

	constant	startmelody = 0
	constant	playmelody = 1
	constant	lastnote = 2
	constant	embelflag = 3


octstate	res	1
octbeat		res	1
chord		res	4
chordvel	res	1
choice		res	1
chordcnt	res	1
chordbass	res	1
bassnote	res	1
melodynote	res	1
legatonote	res	1
melodycnt	res	1
melodyoffset	res	1
melodyvel	res	1

velsave		res	1
embelcnt	res	1
offsave		res	1


PROG1	CODE
octaInit	
	global octaInit
	settempo tempo
	banksel	octstate
	clrf	octstate
	clrf	octbeat
	bsf	octbeat,0	; set to fire on first shot
	clrf	melodycnt
	bsf	melodycnt,0
	clrf	legatonote
	clrf	chord
	clrf	chord+1
	clrf	chord+2
	clrf	chord+3
	return

checkOcta
	global	checkOcta
	btfss	timermask,beattimer
	call	dobeat
	btfss	timermask,chordtimer
	call	dochord
	btfss	timermask,melodytimer
	call	domelody
	btfss	timermask,legatotimer
	call	dolegato
	btfss	timermask,basstimer
	call	dobass
	return

dobeat	
	stimf	beattimer,__quart,1,tlow	; set the next beat	
	banksel	octbeat
	decf	octbeat,f
	btfss	STATUS,Z		; are we on the first beat?
	goto	replaychord		; no just replay chord
	movlw	numbeats
	movwf	octbeat			; reset the beat counter
	clrf	chordcnt		; clear the chord note counter
	randnum	chordlow,chordhigh,1	; get the bass of the scale
	movwf	chordbass		; store it
skipbass
	movlw	chord			; get the address of the chord
	movwf	FSR			; set the chord port
clp1
	call	getrand			; get a randnum number
	andlw	1			; just need one random bit
	banksel	chordcnt		; 
	addwf	chordcnt,w		; add the bit to the cnt
	call	octanotes		; get the scale offset
	addwf	chordbass,w		; add it to the scale bass
	movwf	INDF			; store it in the chord
	incf	FSR,f			; bump the chord
	incf	chordcnt,f		; bump the chord count by two
	incf	chordcnt,f		; by two
	movlw	chord+4
	subwf	FSR,w
	btfss	STATUS,Z		; yes?
	goto	clp1			; no get the next note
	call	playbass
	movlw	firstvel		; get the velocity for the chord
	movwf	chordvel		; save it
	randnum	0,D'10',1
	btfsc	STATUS,Z
	goto	playchord
	bsf	octstate,playmelody
	bsf	octstate,startmelody
	randnum	0,D'10',1
	btfss	STATUS,Z
	goto	one8
	movlw	__quart
	goto	setmtimer
one8
	movlw	__8th
setmtimer
	movwf	tmp1
	stimf	melodytimer,tmp1,0,tlow;
	goto	playchord		; play the chord
replaychord
	movlw	followvel		; get the off beat velocity
	movwf	chordvel		; save it
playchord
	; we can't use the indirect pointer since midpnl does
	banksel	chord
	midpnl	chord,0,chordvel,0
	banksel	chord
	midpnl	chord+1,0,chordvel,0
	banksel	chord
	midpnl	chord+2,0,chordvel,0
	banksel	chord
	midpnl	chord+3,0,chordvel,0
	banksel	chord
	randnum	0,__64th,1
	addlw	(__quart-(__64th/2))
	movwf	tmp1
	stimf	chordtimer,tmp1,0,tlow
	banksel	chord
	return


dochord
	clrf	chordvel
	goto	playchord


domelody
	btfss	octstate,playmelody
	return	
	btfss	octstate,startmelody
	goto	nextnote
	bcf	octstate,startmelody
	randnum	D'2',D'5',1
	banksel	melodycnt
	movwf	melodycnt
	randnum	D'5',D'20',1
	banksel	melodyoffset
	movwf	melodyoffset
	randnum	melodyvelstart,melodyvelend,1
	movwf	melodyvel
	call	donote
	stimf	melodytimer,__8th,1,tlow
	return	

donote	
	movf	melodyoffset,w
	call	octanotes
	addlw	D'12'
	addwf	chordbass,w
	movwf	melodynote
	midpnl	melodynote,0,melodyvel,0
	banksel	octstate
	return

nextnote
	movf	melodycnt,f
	btfsc	STATUS,Z
	goto	dolastnote
	btfsc	octstate,embelflag
	goto	embelnext
	randnum	0,D'10',1
	btfsc	STATUS,Z
	goto	embellishstart
	movf	melodyoffset,w
	movwf	legatonote
	stimf	legatotimer,__16th,1,tlow
	randnum	1,5,1
	subwf	melodyvel,f
	decf	melodycnt,f
	decf	melodyoffset,f
	call	donote
	randnum	0,__64th,1
	addlw	(__8th-(__64th/2))
	movwf	tmp1
	stimf	melodytimer,tmp1,0,tlow
	return

dolastnote
	btfsc	octstate,lastnote
	goto	stopit
	bsf	octstate,lastnote	
	randnum	D'0',D'20',1
	movwf	melodyoffset
	movlw	melodyvellast
	movwf	melodyvel
	call	donote
	stimf	melodytimer,__quart,1,tlow
	return

stopit
	bcf	octstate,lastnote
	bcf	octstate,playmelody
	movf	melodyoffset,w
	movwf	legatonote
	stimf	legatotimer,__16th,1,tlow
	return

dolegato
	movf	legatonote,f
	btfsc	STATUS,Z
	return
	movf	melodyoffset,w
	movwf	offsave
	movf	legatonote,w
	movwf	melodyoffset
	movf	melodyvel,w
	movwf	velsave
	clrf	melodyvel
	call 	donote
	movf	velsave,w
	movwf	melodyvel
	movf	offsave,w
	movwf	melodyoffset
	clrf	legatonote
	return

playbass
	randnum	0,D'3',1
	btfss	STATUS,Z
	return
	movlw	D'24'
	subwf	chord,w
	movwf	bassnote
	midpnl	bassnote,0,bassvel,1
	banksel	bassnote
	movlw	D'12'
	addwf	bassnote,f
	midpnl	bassnote,0,bassvel,1
	banksel	bassnote
	movlw	_whole
	call	getlowticks
	movwf	tmp1
	stimf	basstimer,tmp1,0,tlow
	movlw	_whole
	call	gethighticks
	stimf	basstimer,tmp1,0,thigh
	return

dobass
	movf	bassnote,f
	btfsc	STATUS,Z
	return
	midpnl	bassnote,0,0,1
	banksel bassnote
	movlw	D'12'
	subwf	bassnote,f
	midpnl	bassnote,0,0,1
	banksel bassnote
	clrf	bassnote
	return
	
	
	

	

embellishstart
	bsf	octstate,embelflag
	clrf	embelcnt
	movf	melodyoffset,w
	movwf 	offsave
embelnext
	movf	melodyvel,w
	movwf	velsave
	clrf	melodyvel
	call 	donote
	movf	velsave,w
	movwf	melodyvel
	movf	offsave,w
	movwf	melodyoffset
	movf	embelcnt,w
	andlw	1
	subwf	melodyoffset,f	
	call	donote
	stimf	melodytimer,__32th,1,tlow
	incf	embelcnt,f
	btfss	embelcnt,2
	return
	decf	melodycnt,f
	bcf	octstate,embelflag
	return

	
	
	

	
	
	

	
	
JUMPS7	CODE
	table	octanotes
	retlw	D'0'	
	retlw	D'1'
	retlw	D'3'
	retlw	D'4'
	retlw	D'6'
	retlw	D'7'
	retlw	D'9'
	retlw	D'10'
	retlw	D'12'
	retlw	D'13'
	retlw	D'15'
	retlw	D'16'
	retlw	D'18'
	retlw	D'19'
	retlw	D'21'
	retlw	D'22'
	retlw	D'24'
	retlw	D'25'
	retlw	D'27'
	retlw	D'28'
	retlw	D'30'
	retlw	D'31'
	retlw	D'33'
	retlw	D'34'
	retlw	D'36'

	END
