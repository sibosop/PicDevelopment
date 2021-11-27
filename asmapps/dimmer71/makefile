# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o inter.o dimmer71.o timers.o a2d.o \
			 16c71.lkr
INCS = timers.inc dimmer71.inc a2d.inc

all:	dimmer71

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

dimmer71: $(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make of $@ successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


startup.o: $(INCS)
inter.o: $(INCS)
dimmer71.o: $(INCS)
timers.o: $(INCS)
a2d.o: $(INCS)

