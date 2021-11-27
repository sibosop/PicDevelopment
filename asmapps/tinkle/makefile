# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	= point.o upstruct.o phrases.o inter.o main.o midi.o math.o startup.o \
	timers.o uart.o octa.o 16c73b.lkr 

INCS =  serial.inc ../shared/macros.inc upstruct.inc midi.inc \
	../shared/queue.inc tinkle.inc timers.inc math.inc 

all:	tinkle

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

tinkle:	$(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo DUN DUDE!!
	

.asm.o:
	mpasmwin /o+ $*.asm /q+




phrases.asm:	phrases.pre phrases.dat phrases.prl
	perl -w $*.prl < phrases.dat > $@
	


inter.o:	$(INCS)
main.o:		$(INCS) octa.inc point.inc
midi.o:		$(INCS)
upstruct.o:	$(INCS) phrases.inc
math.o:		$(INCS)
startup.o:	$(INCS)
timers.o:	$(INCS)
uart.o:		$(INCS)
octa.o:		$(INCS) octa.inc
phrases.o:	upstruct.inc timers.inc
point.o:	$(INCS)	point.inc

