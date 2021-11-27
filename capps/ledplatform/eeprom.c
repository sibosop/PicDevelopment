
#include "eeprom.h"
UInt8 readEEPROM(UInt8 address){
    EECON1=0;                   //ensure CFGS=0 and EEPGD=0 
    EEADR = address;
    RD = 1;
    return(EEDATA);
}
 
void writeEEPROM(UInt8 address,UInt8 data){
#if 1
	EEPROM_WRITE(address,data);
#else
	UInt8 SaveInt;
    SaveInt=INTCON;             //save interrupt status
    EEADR = address;            //setup Address
    EEDATA = data;              //and data
	EEPGD = 0;
	WREN = 1;
    GIE=0;           //No interrupts
    EECON2 = 0x55;              //required sequence #1
    EECON2 = 0xaa;              //#2
    WR = 1;          //#3 = actual write
    INTCON=SaveInt;             //restore interrupts
    while(!EEIF);      //wait until finished
    WREN = 0;        //disable write to EEPROM
#endif
}
