# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	mb2.o 16c71.lkr

all:	mb2

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

mb2:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+



