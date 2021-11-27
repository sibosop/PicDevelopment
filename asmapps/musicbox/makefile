# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	musicbox.o 16c71.lkr

all:	musicbox

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

musicbox:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+



