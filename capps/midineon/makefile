
INCDIR = ../include
LKRDIR = 

PROCESSOR = 16F876
LIBDIR = ../$(PROCESSOR)
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
	midineon.obj midihand.obj



		

TARGET	= midineon.hex

LIBRARIES = \
	$(LIBDIR)/$(PROCESSOR).lib

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

midineon.obj:	$(INCLUDES) $(LIBRARIES)
midihand.obj:	$(INCLUDES) $(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

