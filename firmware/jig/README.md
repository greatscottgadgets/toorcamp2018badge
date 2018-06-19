# ToorCamp 2018 Badge Test Jig Firmware

This is the firmware for a GreatFET to run the test/flashing jig.

To build this firmware you need the badge firmware in the form of a C array
```
unsigned char badge_firmware[] = {
  ...
};
unsigned int badge_firmware_len = ...;
```
The array and length should be in a header file called `badge_firmware.h`

Create the file using `xxd -i <firmware file>.bin > badge_firmware.h`
