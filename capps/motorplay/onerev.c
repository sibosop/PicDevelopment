#include 	<pic.h>
#include	<sys.h>
#include 	"types.h"
#include	"onerev.h"

bank2	UInt16 oneRevCounts[NUM_ONE_REVS];
typedef enum {
	PTA = 1,
	PTC
} PortType;

typedef struct _oneRevTab {
	PortType	portType;
	UInt8		mask;
} OneRevTab;

const OneRevTab	oneRevTab[] = {
	{ PTA, 1 },
	{ PTA, 2 },
	{ PTA, 4 },
	{ PTA, 8 },
	{ PTA, 0x20 },
	{ PTC, 1 },
	{ PTC, 2 },
	{ PTC, 4 },
	{ PTC, 8 },
	{ PTC, 0x10 },
	{ PTC, 0x20 },
	{ PTC, 0x40 },
	{ PTC, 0x80 }
};

void
initOneRev() {
	int i;
	for (i = 0; i < NUM_ONE_REVS; ++i) {
		oneRevCounts[i] = 0;
		PORTA = 0x00;
		PORTC = 0x00;
	}
}

void
checkOneRev() {
	int i;
	for ( i = 0; i < NUM_ONE_REVS; ++i ) {
		if ( oneRevCounts[i] ) {
			if ( !--oneRevCounts[i] ) {
				if ( oneRevTab[i].portType == PTA ) {
					PORTA &= ~(oneRevTab[i].mask);
				} else {
					PORTC &= ~(oneRevTab[i].mask);
				}
			}
		}
	}
}

void
startOneRev(UInt8 m) {
	if ( oneRevTab[m].portType == PTA )
		PORTA |= (oneRevTab[m].mask);
	else
		PORTC |= (oneRevTab[m].mask);
	oneRevCounts[m] = ONE_REV_PULSE_COUNT;
}

