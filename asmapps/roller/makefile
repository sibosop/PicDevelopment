# MAKEFILE
#
#

OBJECTS	= inter.o main.o startup.o \
	timers.o dcmotor.o 16c73b.lkr 

INCS =  ../shared/macros.inc \
	timers.inc dcmotor.inc roller.inc

all:	roller

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

roller:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+






inter.o:	$(INCS)
main.o:		$(INCS) 
startup.o:	$(INCS)
timers.o:	$(INCS)
dcmotor.o:	$(INCS) dcmotor.inc
