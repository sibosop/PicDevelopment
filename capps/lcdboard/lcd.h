#ifndef LCD_H
#define LCD_H


void lcdInitDisplay1();
void lcdInitDisplay2();
void lcdClearDisplay();
void lcdSendMessage(const char *msg);
void lcdSetCursor(UInt8 x, UInt8 y);
void lcdSendDecimalNum(UInt8 n);
void lcdEnableDisplay(UInt8 on);
void lcdEnableCursor(UInt8 on);
void lcdEnableBlink(UInt8 on);

#endif
