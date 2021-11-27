# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o main.o inter.o uartsupp.o 16c73b.lkr 

all:	testuart

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

testuart:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+



