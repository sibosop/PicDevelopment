#ifndef A2D_H
#define A2D_H
#include "types.h"
#define RIGHT_JUSTIFY
void  initA2D();
void  checkA2D(void);
UInt8 getA2D(UInt8 chan);

#define DECLARE_A2D_VALS(x) \
	const	UInt8	numA2DChannels = x; \
	UInt16	A2DVals[x];  \
	const 	UInt8	useA2DRef = 0

#define DECLARE_A2D_REF \
	const	UInt8	numA2DChannels = 5; \
	UInt16	A2DVals[5]; \
	const   UInt8	useA2DRef = 1

extern UInt16 A2DVals[];
extern const UInt8	numA2DChannels;
extern const UInt8	useA2DRef;

#endif
