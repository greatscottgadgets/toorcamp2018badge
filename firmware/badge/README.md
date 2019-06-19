ToorCamp 2018 Badge firmware may be compiled with mspgcc:

http://mspgcc.sourceforge.net/

build instructions:
```
$ make
```

flash with GreatFET:
```
$ gf msp430 -e
$ gf msp430 -a 0xf800 -f fireflies.bin
$ gf msp430 -r
```

flash with GoodFET:
```
$ goodfet.msp430 erase
$ goodfet.msp430 flash fireflies.hex
$ goodfet.msp430 run
```
