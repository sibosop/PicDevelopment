#ifndef LCD_H
#define LCD_H


UInt8 checkLcd();
void lcdClearDisplay();
void lcdSendMessage(const char *msg);
void lcdSetCursor(UInt8 x, UInt8 y);
void lcdSendDecimalNum(UInt8 n);
void lcdEnableDisplay(UInt8 on);
void lcdEnableCursor(UInt8 on);
void lcdEnableBlink(UInt8 on);
void lcdEnableScroll(UInt8 on);
void lcdSendData(UInt8 d);
void lcdMoveDisplay(UInt8 d);
void lcdEnableShift(UInt8 d);
void lcdSetCursorDir(UInt8 d);
void lcdDisableShift();
void lcdSend2DigitNum(UInt8 d);
void lcdSendCommand(UInt8 c);
#endif
