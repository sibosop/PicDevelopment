#ifndef VOICEPLY_H
#define VOICEPLY_H
#ifndef  IN_VOICEPLY
	extern state
	extern tmp1
	extern tmp2
	extern tmp3
	extern tmp4
#endif

#ifndef	IN_VOICE
	extern	checkPud
	extern	checkEom
	extern	checkVoc
	extern 	initVoc
	extern	startVoice
	extern	stopVoice
#endif
	constant	pudwaittimer = 0
	constant	eomTimer     = 1
	constant	startupTimer    = 2
	
; state bits
	constant	Alarm = 0
	constant	Eom   = 1
	constant	PudWait = 2
	constant	Startup = 3

#endif
