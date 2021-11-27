#ifndef MINPUT_INC
#define MINPUT_INC

	ifndef	isGlob
	extern	state
	endif
	ifndef  IN_VOICE
	extern	initVoc
	extern	checkPud
	extern 	checkEom
	extern	checkVoc
	extern	handleOverflow
	endif
	constant	alarm	    = 0
	constant	charready   = 1
	constant	eom         = 2
	constant	pudwait	    = 3
	constant	overflow	    = 4
	constant	startup		= 5

	constant	clockrate = 4	;MHZ

	constant	pudwaittimer = 1
	constant	eomTimer     = 2
	constant	randTimer    = 3
	constant	INDFHI	= 0x80 	;needed for bankselect 

#endif
