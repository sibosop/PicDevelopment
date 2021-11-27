# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	ad71.o 16c71.lkr

all:	ad71

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

ad71:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+



