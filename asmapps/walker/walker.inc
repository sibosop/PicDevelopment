#ifndef WALKER_H
#define WALKER_H
#ifndef  IN_WALKER
	extern state
	extern tmp1
	extern tmp2
	extern tmp3
	extern tmp4
#endif
	
; state bits
	constant	Alarm = 0
	constant	ForwardFlag = 1
	constant	LeftFlag = 2

	constant	ForwardLeft  = 0x10 
	constant	BackwardLeft = 0x20
	constant	ForwardRight = 0x80 
	constant	BackwardRight = 0x40
	constant	LeftMask     = 0x30
	constant	RightMask    = 0xc0

	constant	MotorTimer = 0
	constant	FrontDebounce = 1
	constant	BackDebounce = 2

	constant	FrontSensor = 0
	constant	BackSensor  = 1
	constant	SwitchTime = D'7'
	constant	DirLimit = D'240'

#endif
