#ifndef RANDOM_H
#define RANDOM_H
#include "types.h"
UInt16 random(UInt16 low, UInt16 high);
void initRandom(void);
void mixRandom(void);
#endif
