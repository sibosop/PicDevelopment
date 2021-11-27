#ifndef PIC_PWM_H
#define PIC_PWM_H

void initPwm();

#define SET_PWM1(x) \
	CCPR1L = x >> 2; \
	CCP1X = x & 0x2 ? 1 : 0; \
	CCP1Y = x & 0x1

#define SET_PWM2(x) \
	CCPR2L = x >> 2; \
	CCP2X = x & 0x2 ? 1 : 0; \
	CCP2Y = x & 0x1



#endif
