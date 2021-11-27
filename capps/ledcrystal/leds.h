#ifndef LEDS_H
#define LEDS_H
#include <pic.h>

#define NUM_LEDS	1
#define MIN_LED_SPEED 10
#define MAX_LED_SPEED 30
#define LED_INTERVAL 3


void ledInit();
void checkLeds(void);	
#endif
