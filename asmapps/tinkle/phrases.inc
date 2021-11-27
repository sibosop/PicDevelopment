#ifndef PHRASES_INC
#define PHRASES_INC
#ifndef	IN_PHRASES
	extern jumpphraselow
	extern jumpphrasehigh
	extern getphrasecount
	extern phraseptr
	extern phrasenum

jphrl	macro	
	pagesel	jumpphraselow
	call	jumpphraselow
	pagesel	getphrasecount
	endm

jphrh	macro	
	pagesel	jumpphrasehigh
	call	jumpphrasehigh
	pagesel	getphrasecount
	endm


#endif
	
	constant	_C = D'0'
	constant	_D = D'2'
	constant	_E = D'4'
	constant	_F = D'5'
	constant	_G = D'7'
	constant	_A = D'9'
	constant	_B = D'11'
#endif
