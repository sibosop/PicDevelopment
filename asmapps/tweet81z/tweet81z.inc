#ifndef tweet81z_INC
#define tweet81z_INC

	ifndef	isGlob
	extern	state
	endif
	constant	alarm	  = 0
	constant	charready = 1
	constant	clockrate = 4	;MHZ
	constant	INDFHI	= 0x80 	;needed for bankselect 
#ifndef IN_NOTES
	extern	initNotes;
	extern	checkNotes;
#endif
#endif
