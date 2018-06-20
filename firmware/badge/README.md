ToorCamp 2018 Badge firmware may be compiled with mspgcc:

http://mspgcc.sourceforge.net/

build instructions:
```
$ make
$ goodfet.msp430 erase
$ goodfet.msp430 flash fireflies.hex
$ goodfet.msp430 run
```
