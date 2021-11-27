# MAKEFILE
#

OBJECTS	= mparse.o mhandler.o inter.o main.o startup.o \
	uart.o math.o timers.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc midi.inc \
	math.inc minput.inc uart.inc voicerec.inc timers.inc

all:	voicerp

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

voicerp: $(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+






inter.o:	$(INCS)
main.o:		$(INCS) mparse.inc
startup.o:	$(INCS)
math.o:	$(INCS)
uart.o:		$(INCS)	queue.inc
midi.o:		$(INCS)
mparse.o:	$(INCS) mparse.inc
mhandler.o:	$(INCS) 
timers.o:	$(INCS)
