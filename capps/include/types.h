#ifndef TYPES_H
typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned long  UInt32;
typedef bit	Boolean;

//typedef signed char  Int8;
//typedef short Int16;
//typedef long  Int32;

#define MIN(x,y) 	((x) < (y) ? (x) : (y))
#define MAX(x,y) 	((x) > (y) ? (x) : (y))

#if defined(_16C71) 
#define bank1 
#endif
#endif
