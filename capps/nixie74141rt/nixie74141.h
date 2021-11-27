#ifndef nixie74141_h
#define nixie74141_h
#include "bank.h"
#include <stdint.h>

#define NUM_METERS	2

#define PERIOD	360
#define HIGH_SET	360
#define TICK_OFFSET	40
#define LOW_TICK	(TICK_OFFSET)
#define HIGH_TICK	(HIGH_SET-TICK_OFFSET)

#define NIXIE_HOUR_MSB	3
#define NIXIE_HOUR_LSB  2
#define NIXIE_MIN_MSB	1
#define NIXIE_MIN_LSB	0
extern uint16_t duty1;
extern uint8_t nixieVals[4];

void versionInit();
void checkButtons();
void initButtons();

extern uint8_t	hours;
extern uint8_t	minutes;
extern uint16_t 	seconds;
extern void setMinutes();
extern void setSeconds();
extern void setHours();
extern void displayHours();
extern void displayMinutes();
extern void displaySeconds();
#endif
