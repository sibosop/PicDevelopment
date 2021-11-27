# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o main.o inter.o 16c73b.lkr 

INCS = ../shared/macros.inc serial.inc

all:	mscape

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

mscape:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


main.o:	$(INCS)
startup.o:	$(INCS)
inter.o:	$(INCS)

