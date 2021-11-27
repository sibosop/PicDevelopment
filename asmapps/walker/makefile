# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o inter.o walker.o timers.o 16c71.lkr
INCS = timers.inc walker.inc

all:	walker

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

walker: $(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make of $@ successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


startup.o: $(INCS)
inter.o: $(INCS)
walker.o: $(INCS)
timers.o: $(INCS)

