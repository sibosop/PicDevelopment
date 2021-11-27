# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	inter.o main.o midi.o moron.o startup.o uart.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc moron.inc midi.inc ../shared/queue.inc

all:	mscape

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

mscape:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


inter.o:	$(INCS)
main.o:		$(INCS)
midi.o:		$(INCS)
moron.o:	$(INCS)
startup.o:	$(INCS)
uart.o:		$(INCS)

