
INCDIR = 
LKRDIR = 
LIBDIR = 

PROCESSOR = 16C73B
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM


CFLAGS = -g -d$(PLATFORM) -d$(PROCDEF) -w3 -$(PROCESSOR) -ASMLIST \
			-FAKELOCAL -ICD

LFLAGS = -g -$(PROCESSOR) -PSECTMAP -N -m$@.map -FAKELOCAL -ICD
 
CC = picc
LINK = picc

OBJECTS = \
	ctest.obj


		

TARGET	= elock

INCLUDES =  \
	machine.h

all: $(TARGET)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS)
	$(LINK) $(LFLAGS) -O$@.hex $(OBJECTS)
	echo $@ complete

ctest.obj:	$(INCLUDES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

