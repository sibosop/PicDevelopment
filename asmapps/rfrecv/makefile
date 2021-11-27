# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	= inter.o main.o startup.o \
	timers.o uart.o math.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc midi.inc \
	rfrecv.inc timers.inc math.inc uart.inc 

all:	rfrecv

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

rfrecv:	$(OBJECTS)
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
