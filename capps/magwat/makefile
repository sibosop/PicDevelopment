
INCDIR = ../include
LKRDIR = 
LIBDIR = ../c73blib

PROCESSOR = 16C73B
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM

CLOCKRATE = 4


CFLAGS = -g -o -i$(INCDIR) -d$(PLATFORM) -d$(PROCDEF) \
			 -w3 -$(PROCESSOR) -ASMLIST \
			-FAKELOCAL -ICD -dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = -g -$(PROCESSOR)  -N -m$*.map 
 
CC = picc
LINK = picc

OBJECTS = \
	magwat.obj 



		

TARGET	= magwat.hex

LIBRARIES = \
	$(LIBDIR)/c73b.lib

INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/midi.h \
	$(INCDIR)/random.h \
	$(INCDIR)/serial.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h

all: $(TARGET)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS) $(LIBRARIES)
	$(LINK) $(LFLAGS) -O$@ $(OBJECTS) $(LIBRARIES)
	echo $@ complete

a2d.obj:	$(INCLUDES) $(LIBRARIES)
magwat.obj:	$(INCLUDES) $(LIBRARIES)
interrupt.obj:	$(INCLUDES) $(LIBRARIES)
midi.obj: $(INCLUDES) $(LIBRARIES)
random.obj: $(INCLUDES) $(LIBRARIES)
serial.obj: $(INCLUDES) $(LIBRARIES)
timers.obj: $(INCLUDES) $(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

