# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	step71.o 16c71.lkr

all:	step71

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

step71:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+



