
INCDIR = ../include
LKRDIR = 

PROCESSOR = 16C71
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM
LIBDIR = ../$(PROCESSOR)

CLOCKRATE = 4


CFLAGS = -o -i$(INCDIR) -d$(PLATFORM) -d$(PROCDEF) \
			 -w3 -$(PROCESSOR) -ASMLIST \
			-dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = -$(PROCESSOR)  -N -m$*.map
 
CC = picc
LINK = picc

OBJECTS = \
	tweet71.obj


		

TARGET	= tweet71.hex

LIBRARIES =  \
	$(LIBDIR)/$(PROCESSOR).lib


INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/random.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h

all: $(TARGET)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS) $(LIBRARIES)
	$(LINK) $(LFLAGS) -O$@ $(OBJECTS) $(LIBRARIES)
	echo $@ complete

miditest.obj:	$(INCLUDES) $(LIBRARIES)
mhandler.obj:	$(INCLUDES)	$(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

