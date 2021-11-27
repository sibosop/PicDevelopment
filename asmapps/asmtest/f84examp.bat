@rem
@rem        File: EXAM.BAT
@rem
@rem    Used to compile the MPLINK example.
@rem
@rem assumes that you can find the p16F84A.inc include file
@rem  in the MPASM install directory. the 16F84a.lkr file
@rem  is also copied from the install, but can be modified....

mpasmwin /o+ f84examp.asm /q+
mplink -o f84examp.out -m f84examp.map f84examp.o 16F84a.lkr 
@pause

