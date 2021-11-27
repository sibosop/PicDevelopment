# MAKEFILE
# rules for ROBOCAR PIC
#  (c) M.Schippling Y2k+1
#
#

OBJECTS	=	startup.o inter.o voiceply.o timers.o voice.o math.o a2d.o \
			 16c71.lkr
INCS = timers.inc voiceply.inc math.inc a2d.inc

all:	voiceply

clean:
	rm -f *.err *.hex *.o *.cod *.map *.lst *.out

voiceply: $(OBJECTS)
	mplink -o $*.out -m $*.map $(OBJECTS)
	@echo make of $@ successful
	

.asm.o:
	mpasmwin /o+ $*.asm /q+


startup.o: $(INCS)
inter.o: $(INCS)
voiceply.o: $(INCS)
voice.o: $(INCS)
timers.o: $(INCS)
math.o: $(INCS)
a2d.o: $(INCS)

