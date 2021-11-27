# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	= inter.o startup.o  main.o a2d.o notes.o \
	timers.o uart.o math.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc midi.inc \
	tweet81z.inc timers.inc math.inc uart.inc a2d.inc 

all:	tweet81z

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

tweet81z:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+






inter.o:	$(INCS)
main.o:		$(INCS) 
startup.o:	$(INCS)
timers.o:	$(INCS)
math.o:		$(INCS)
uart.o:		$(INCS)	queue.inc
midi.o:		$(INCS)
notes.o:		$(INCS)
a2d.o:		$(INCS)
