# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	moron.o 16c73b.lkr 
INCS	=	../shared/macros.inc

all:	moron

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

moron:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


moron.o:	$(INCS)
