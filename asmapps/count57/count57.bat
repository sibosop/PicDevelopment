@rem
@rem        File: EXAM.BAT
@rem
@rem    Used to compile the MPLINK example.
@rem
@rem assumes that you can find the p16F84A.inc include file
@rem  in the MPASM install directory. the 16F84a.lkr file
@rem  is also copied from the install, but can be modified....

mpasmwin /o+ count57.asm /q+
mplink -o count57.out -m count57.map count57.o 16c57c.lkr 
@pause

