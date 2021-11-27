
INCDIR = ../include
LKRDIR = 
LIBDIR = 
COMDIR = ../common

PROCESSOR = 16C71
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM

CLOCKRATE = 4


CFLAGS = -i$(INCDIR) -d$(PLATFORM) -d$(PROCDEF) \
			-w3 -$(PROCESSOR) -ASMLIST \
			-dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = r
 
CC = picc
LINK = libr

OBJECTS = \
	a2d.obj \
	a2dmem.obj \
	inter.obj  \
	intermem.obj  \
	random.obj \
	timem.obj \
	timers.obj



		

TARGET	= 16c71.lib

INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \

all: $(TARGET) 

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym *.lib \
	a2d.c a2dmem.c random.c inter.c intermem.c timers.c timem.c

$(TARGET): $(OBJECTS)
	$(LINK)  $(LFLAGS) $@ $(OBJECTS)
	echo $@ complete

inter.obj:	$(INCLUDES)
intermem.obj:	$(INCLUDES)
a2d.obj:	$(INCLUDES)
#ctest.obj:	$(INCLUDES)
#hinit.obj:	$(INCLUDES)
#mhandler.obj: $(INCLUDES)
#midiout.obj: $(INCLUDES)
#mparse.obj: $(INCLUDES)
#noteon.obj: $(INCLUDES)
random.obj: random.c $(INCLUDES)
#serial.obj: $(INCLUDES)
#timeclk.obj: $(INCLUDES)
timers.obj: $(INCLUDES)


a2d.c: $(COMDIR)/a2d.c
	cp $? $@

a2dmem.c: $(COMDIR)/a2dmem.c
	cp $? $@

inter.c: $(COMDIR)/inter.c
	cp $? $@

intermem.c: $(COMDIR)/intermem.c
	cp $? $@

random.c: $(COMDIR)/random.c
	cp $? $@

timers.c: $(COMDIR)/timers.c
	cp $? $@

timem.c: $(COMDIR)/timem.c
	cp $? $@


.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

