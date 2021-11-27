#ifndef LEDS_H
#define LEDS_H
#include <pic.h>



void ledCheck(void);
void ledInit(UInt8 size);	
void ledClear();
typedef struct _Rgb {
	UInt8 r;
	UInt8 g;
	UInt8 b;
} Rgb;
void ledSet(UInt8 n, Rgb *val);
void ledGet(UInt8 n, Rgb *val);
#endif
