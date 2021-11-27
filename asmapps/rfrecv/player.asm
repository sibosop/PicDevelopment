	processor	16C73B
	#define		IN_UPSTRUCT
	#include	P16C73B.INC
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include 	minput.inc
	#include	midi.inc
	#include	math.inc
	#include	timers.inc


#define MIDI_NOTE_TEST

#ifdef MIDI_NOTE_TEST
	constant	tempo	     = D'60'
	constant	uptimer 	= 0

	constant	onflag	 = 0
	constant	waitflag = 1
RAM2	UDATA	
pstate	res	1
cnt	res	1

PROG1	CODE

playerinit
	global playerinit
	settempo tempo
	banksel	pstate
	clrf	pstate
	movlw	D'10'
	movwf	cnt
	bsf	pstate,waitflag
	bsf	pstate,onflag
	return

checkplayer:
	global	checkplayer
	btfss	timermask,uptimer	; see if in wait
	goto	playit
	return

playit
	banksel	tmp1
	movlw	low __quart
	movwf	tmp1	
	stimf	uptimer,tmp1,0,tlow
	btfss	pstate,onflag
	goto	noteoff
	call	donote
	banksel	pstate
	bcf	pstate,onflag
	return
noteoff
	call	donote
	banksel	pstate
	bsf	pstate,onflag
	return
	
donote
	btfss	pstate,waitflag
	goto	dn1
	decf	cnt,f
	btfsc	STATUS,Z
	bcf	pstate,waitflag
	return

dn1
	btfss	pstate,onflag
	goto	dnoff
	midpnl	D'60',1,D'100',1	; play it
	return
dnoff
	midpnl	D'60',1,D'0',1
	return

#endif
	end
