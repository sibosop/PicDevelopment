;
; midi macros, externs
;

	nolist
	constant midichannel=0
	constant midinoteon=0x90
	
	ifndef	IN_MIDI_FILE
	extern	midiinit
	endif

midpnl	macro	note,litnote, velocity, litvel
	movlw	(midichannel | midinoteon)
	call	uartwrite
	if (litnote == 1 )
	movlw	note
	else
	banksel	note
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
	list

