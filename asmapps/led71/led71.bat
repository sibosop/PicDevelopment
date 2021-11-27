@rem
@rem        File: EXAM.BAT
@rem
@rem    Used to compile the MPLINK example.
@rem
@rem assumes that you can find the p16F84A.inc include file
@rem  in the MPASM install directory. the 16F84a.lkr file
@rem  is also copied from the install, but can be modified....

mpasmwin /o+ led71.asm /q+
mplink -o led71.out -m led71.map led71.o 16C71.lkr 
@pause

