

; midi.asm

; sub-routine entry point: inituart
;			   writeuart
; called from startup.asm with base registers saved
;

; define chip to be used and include the standard stuff
	processor	16C73B
	#define	IN_MIDI_FILE
	#include	P16C73B.INC
	#define		UARTSUPP
	#include	serial.inc
	#include	"..\shared\macros.inc"
	#include	"..\shared\queue.inc"
	#include	tinkle.inc
	#include	midi.inc
	

; define local registers, etal

	UDATA	

; Put it in ROM PAGE0
PROG1	CODE

;
; initialize the midi subsystem
;
midiinit
	global midiinit
	return



	END
