#ifndef EEPROM_H
#define EEPROM_H

#include <pic.h>
#include <types.h>

extern UInt8	readEEPROM(UInt8 address);
extern void	writeEEPROM(UInt8 address, UInt8 data);
#endif


