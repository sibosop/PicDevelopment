;
; midi macros, externs
;

	nolist
	constant midichannel=0
	constant midinoteon=0x90
	constant midiprogchange=0xC0
	constant midicmode=0xB0

;
; channel mode messages
;
	constant cmalloff=0x7B
	
	ifndef	IN_MIDI_FILE
	extern	midiinit
	endif

midpnl	macro	note,litnote, velocity, litvel
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
	endm

midpc	macro	prog,litnote
	movlw	(midichannel | midiprogchange)
	call	uartwrite
	if (litnote == 1 )
	movlw	prog
	else
	banksel	prog		; since banksel will always put it in zero
	movf	prog,w
	endif
	call	uartwrite
	endm


midiaoff macro	
	movlw	(midichannel | midicmode)
	call	uartwrite
	movlw	cmalloff
	call	uartwrite
	movlw	0
	call	uartwrite
	endm

	list

