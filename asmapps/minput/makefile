# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	= mparse.o mhandler.o inter.o main.o startup.o \
	timers.o uart.o math.o player.o a2d.o motor.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc midi.inc \
	timers.inc math.inc minput.inc uart.inc a2d.inc motor.inc

all:	minput

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

minput:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+






inter.o:	$(INCS)
main.o:		$(INCS) mparse.inc
startup.o:	$(INCS)
timers.o:	$(INCS)
math.o:	$(INCS)
uart.o:		$(INCS)	queue.inc
midi.o:		$(INCS)
mparse.o:	$(INCS) mparse.inc
mhandler.o:	$(INCS) 
player.o:	$(INCS)
a2d.o:		$(INCS)
motor.o:	$(INCS) motor.inc
