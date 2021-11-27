
; midi macros, externs
;
#ifndef MIDI_INC
#define MIDI_INC

	nolist
	constant midichannel=0
	constant midinoteon=0x90
	constant midiprogchange=0xC0
	constant midicmode=0xB0

;
; channel mode messages
;
	constant cmalloff=0x7B
	

midpnl	macro	note,litnote, velocity, litvel
	disable
	movlw	(midichannel | midinoteon)
	call	uartwrite
	if (litnote == 1 )
	movlw	note
	else
	banksel	note		; since banksel will always put it in zero
	movf	note,w
	endif
	call	uartwrite
	if ( litvel == 1 )
	movlw	velocity
	else
	banksel	velocity
	movf	velocity,w
	endif
	call	uartwrite
	enable
	endm

midpc	macro	prog,litnote
	disable
	movlw	(midichannel | midiprogchange)
	call	uartwrite
	if (litnote == 1 )
	movlw	prog
	else
	banksel	prog		; since banksel will always put it in zero
	movf	prog,w
	endif
	call	uartwrite
	enable
	endm


midiaoff macro	
	movlw	(midichannel | midicmode)
	call	uartwrite
	movlw	cmalloff
	call	uartwrite
	movlw	0
	call	uartwrite
	endm

midicntl macro	num,litnum,val,litval
	disable
	movlw	(midichannel | midicmode)
	call	uartwrite
	if (litnum == 1 )
	movlw	num
	else
	banksel	num
	movf	num,w
	endif
	call	uartwrite
	if ( litval == 1 )
	movelw	val
	else
	banksel	val
	movf	val,w
	endif
	call	uartwrite
	enable
	endm

	list
#endif

