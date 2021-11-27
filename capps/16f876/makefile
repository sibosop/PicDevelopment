
.SUFFIXES:	.t .c .obj

INCDIR = ../include
LKRDIR = 
LIBDIR = 
COMDIR = ../common

PROCESSOR = 16F876
PROCDEF = _$(PROCESSOR)
PLATFORM = NT_PLATFORM

CLOCKRATE = 4


CFLAGS = -i$(INCDIR) -d$(PLATFORM) \
			-w3 -$(PROCESSOR) -ASMLIST \
			-dCLOCKRATE=$(CLOCKRATE) 

LFLAGS = r
 
CC = picc
LINK = libr

OBJECTS = \
	a2d.obj \
	a2dmem.obj \
	cpress.obj \
	ctrl.obj \
	hinit.obj \
	inter.obj  \
	intermem.obj \
	mhandler.obj \
	midiout.obj \
	mparse.obj \
	noteon.obj \
	playnote.obj \
	pnotemem.obj \
	polykey.obj \
	pgm.obj \
	random.obj \
	rtstart.obj \
	rtstop.obj \
	rtcont.obj \
	serial.obj \
	sermem.obj \
	timeclk.obj \
	timers.obj  \
	timem.obj



		

TARGET	= 16F876.lib

INCLUDES =  \
	$(INCDIR)/a2d.h \
	$(INCDIR)/interrupt.h \
	$(INCDIR)/midi.h \
	$(INCDIR)/playnote.h \
	$(INCDIR)/random.h \
	$(INCDIR)/serial.h \
	$(INCDIR)/timers.h \
	$(INCDIR)/types.h

all: $(TARGET) 

clean:
	-rm -rf *.err *.o *.out *.hex *.lst  *.cod *.rlf \
	*.sdb *.map *.i *.obj *.sym *.lib \
	inter.c random.c a2d.c a2dmem.c timers.c timem.c 

$(TARGET): $(OBJECTS)
	$(LINK)  $(LFLAGS) $@ $(OBJECTS)
	echo $@ complete

	


a2d.obj:	$(INCLUDES)
a2dmem.obj:	$(INCLUDES)
ctrl.obj: $(INCLUDES)
hinit.obj:	$(INCLUDES)
inter.obj: $(INCLUDES)
imtermem.obj: $(INCLUDES)
mhandler.obj: $(INCLUDES)
midiout.obj: $(INCLUDES)
mparse.obj: $(INCLUDES)
noteon.obj: $(INCLUDES)
playnote.obj: $(INCLUDES)
pgm.obj: $(INCLUDES)
polykey.obj: $(INCLUDES)
random.obj: random.c $(INCLUDES)
rtstart.obj: rtstart.c $(INCLUDES)
rtstop.obj: rtstop.c $(INCLUDES)
rtcont.obj: rtcont.c $(INCLUDES)
serial.obj: $(INCLUDES)
sermem.obj: $(INCLUDES)
timeclk.obj: $(INCLUDES)
timers.obj: $(INCLUDES)
timem.obj:	$(INCLUDES)

a2d.c: $(COMDIR)/a2d.c
	cp $? $@

a2dmem.c: $(COMDIR)/a2dmem.c
	cp $? $@

cpress.c: $(COMDIR)/cpress.c
	cp $? $@

ctrl.c: $(COMDIR)/ctrl.c
	cp $? $@

hinit.c:	$(COMDIR)/hinit.c
	cp $? $@

inter.c:	$(COMDIR)/inter.c
	cp $? $@


intermem.c:	$(COMDIR)/intermem.c
	cp $? $@

mhandler.c: $(COMDIR)/mhandler.c
	cp $? $@

midiout.c: $(COMDIR)/midiout.c
	cp $? $@

mparse.c: $(COMDIR)/mparse.c
	cp $? $@

noteon.c: $(COMDIR)/noteon.c
	cp $? $@

playnote.c: $(COMDIR)/playnote.c
	cp $? $@

pnotemem.c: $(COMDIR)/pnotemem.c
	cp $? $@

polykey.c: $(COMDIR)/polykey.c
	cp $? $@

pgm.c: $(COMDIR)/pgm.c
	cp $? $@

random.c: $(COMDIR)/random.c
	cp $? $@

rtstart.c: $(COMDIR)/rtstart.c
	cp $? $@

rtstop.c: $(COMDIR)/rtstop.c
	cp $? $@

rtcont.c: $(COMDIR)/rtcont.c
	cp $? $@

serial.c: $(COMDIR)/serial.c
	cp $? $@

sermem.c: $(COMDIR)/sermem.c
	cp $? $@

timeclk.c: $(COMDIR)/timeclk.c
	cp $? $@

timers.c: $(COMDIR)/timers.c
	cp $? $@

timem.c:	$(COMDIR)/timem.c
	cp $? $@




.c.obj:
	$(CC) -c $(CFLAGS) $*.c
	

