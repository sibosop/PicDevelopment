
INCDIR = ../include
LKRDIR = 
LIBDIR = ../16c71

PROCESSOR = 16C71
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM

CLOCKRATE = 4


CFLAGS = -o -i$(INCDIR) -d$(PLATFORM) -d$(PROCDEF) \
			 -w3 -$(PROCESSOR) -ASMLIST \
			-dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = -$(PROCESSOR)  -N -m$*.map
 
CC = picc
LINK = picc

OBJECTS = \
	actuator.obj 


		

TARGET	= actuator.hex

LIBRARIES =  \
	$(LIBDIR)/16c71.lib


INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h \

all: $(TARGET)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf genrand.c \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS) $(LIBRARIES)
	cp $(LIBDIR)/*.rlf .
	$(LINK) $(LFLAGS) -O$@ $(OBJECTS) $(LIBRARIES)
	echo $@ complete

actuator.obj: $(INCLUDES)

genrand.c: foo
	perl genrand.prl > $@

foo:

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

