#ifndef MENUS_H
#define MENUS_H

#define MENU_NULL	0

typedef void (const *ffp)(void);
typedef UInt8 (const *ffpi)(void);

typedef const struct lookupEntry {
	const char *functionName;
	const ffp	init;
	const ffp	setValue;
	const ffpi	getValue;
} LOOKUP_ENTRY;

typedef struct {
	UInt8	cursor;
	UInt8	value;
} MenuArgs;

extern MenuArgs	menuArgs;

extern LOOKUP_ENTRY	menus[];		// defined by specific menu
extern UInt8 getMenuSize();			// defined by specific menu

extern void checkMenus();
extern void setMenuCursor(UInt8 on);
extern void setMenuValueAtCursor(UInt8 newVal);
extern void setMenuValue(UInt8 pos, UInt8 newVal);
extern void setMenuCursorPos(UInt8 pos);
#endif

