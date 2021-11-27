
INCDIR = ../include
LKRDIR = 
LIBDIR = ../16F876

PROCESSOR = 16F876
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
	dekatron.obj 



		

TARGET	= dekatron.hex

LIBRARIES = \
	$(LIBDIR)/16F876.lib

INCLUDES =  \
	$(INCDIR)/interrupt.h \
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

dekatron.obj:	$(INCLUDES) $(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

