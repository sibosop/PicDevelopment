# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o main.o inter.o uart.o midi.o 16c73b.lkr 

INCS = ../shared/queue.inc serial.inc ../shared/macros.inc moron.inc 

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
uart.o:		$(INCS)
midi.o:		$(INCS)

