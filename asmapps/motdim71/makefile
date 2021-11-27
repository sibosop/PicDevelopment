# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o inter.o motodim.o timers.o math.o a2d.o \
			 16c71.lkr
INCS = timers.inc motodim.inc math.inc a2d.inc

all:	motodim

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

motodim: $(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make of $@ successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


startup.o: $(INCS)
inter.o: $(INCS)
motodim.o: $(INCS)
timers.o: $(INCS)
math.o: $(INCS)
a2d.o: $(INCS)

