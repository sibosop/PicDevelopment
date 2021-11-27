
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
	lcdboard.obj \
	lcd.obj \
	linefunc.obj \
	menus.obj



		

TARGET	= lcdboard.hex

LIBRARIES = \
	$(LIBDIR)/16F876.lib

INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/midi.h \
	$(INCDIR)/random.h \
	$(INCDIR)/serial.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h \
	./linefunc.h \
	./lcd.h \
	./menus.h \
	./lcdboard.h

all: $(TARGET)

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym

$(TARGET): $(OBJECTS) $(LIBRARIES)
	cp $(LIBDIR)/*.rlf .
	$(LINK) $(LFLAGS) -O$@ $(OBJECTS) $(LIBRARIES)
	echo $@ complete

lcdboard.obj:	$(INCLUDES) $(LIBRARIES)
linefunc.obj:	$(INCLUDES) $(LIBRARIES)
lcd.obj:		$(INCLUDES)	$(LIBRARIES)
menus.obj:		$(INCLUDES)	$(LIBRARIES)

.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

