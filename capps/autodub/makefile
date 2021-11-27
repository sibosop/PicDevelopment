
INCDIR = ../include
LKRDIR = 
LIBDIR = ../16F876

PROCESSOR = 16F876
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM

CLOCKRATE = 4


CFLAGS = -g  -i$(INCDIR) -d$(PLATFORM) -d$(PROCDEF) \
			 -w3 -$(PROCESSOR) -ASMLIST \
			-FAKELOCAL -ICD -dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = -g -$(PROCESSOR)  -N -m$*.map -ICD -FAKELOCAL
 
CC = picc
LINK = picc

OBJECTS = \
	autodub.obj 



		

TARGET	= autodub.hex

LIBRARIES = \
	$(LIBDIR)/16F876.lib

INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/midi.h \
	$(INCDIR)/playnote.h \
	$(INCDIR)/random.h \
	$(INCDIR)/serial.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h

all: $(TARGET) $(TEST)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS) $(LIBRARIES)
	cp $(LIBDIR)/*.rlf .
	$(LINK) $(LFLAGS) -O$@ $(OBJECTS) $(LIBRARIES)
	echo $@ complete

autodub.obj:	$(INCLUDES) $(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

