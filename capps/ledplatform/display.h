#ifndef DISPLAY_H
#define DISPLAY_H
#include "types.h"
#include "serial.h"


extern void initDisplay();
extern void displayOn(UInt8 on);
extern void dspSetCursor(UInt8 x, UInt8 y);
extern void dspCursorHome(void);
extern void dspUnderline(UInt8 on);
extern void dspCursorLeft(void);
extern void dspCursorRight(void);
extern void dspBlink(UInt8 on);
extern void dspBackspace(void);
extern void dspClearScreen(void);
extern void dspContrast(UInt8 val);
extern void dspBrightness(UInt8 b);
extern void dspLoadCustomChar(UInt8 spec[9]);
extern void dspMoveDisplayLeft(void);
extern void dspMoveDisplayRight(void);
extern void dspChangeBaud(UInt8 baud);
extern void dspFirmwareVersion();
extern void dspBaud();

extern void dspChar(UInt8 c);
extern void dspCharAt(UInt8 c, UInt8 x, UInt8 y);
extern void dspByte(UInt8 b);
extern void dspString(const char *s);
#endif
