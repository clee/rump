Spaceman Spiff's Commodire 64 USB Keyboard (c64key for short) is
is free software; you can redistribute it and/or modify it under 
the terms of the OBDEV license, as found in the licence.txt file.
                                                              
c64key is distributed in the hope that it will be useful,    
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
OBDEV license for further details.
                                                              

This is the README file for Spaceman Spiff's Commodore 64 USB Keyboard.

Spaceman Spiff's Commodore 64 USB Keyboard (c64key for short) is a
USB HID Keyboard device built into an old Commodore 64 "breadbox", 
allowing the (rather crappy but highly nostalgic) keyboard of a C64
to be used as an input device for e.g. a PC. Of course this allows
the keyboard to be used in conjunction with a C64 emulator to get
the right nostalgic feel, but this use has not been tested, and the
conversion of scan-codes may result in less optimal results in this 
case.

The basis of the project was Objective Development's HIDkeys demo
application, although a great deal of modifications had to be made
in order to scan a proper keyboard matrix, as well as report more 
than one keypress at a time. This also meant the HID report format
had to be altered.

Two different versions of the hardware can be used. One uses an
Atmel ATmega8, whereas the other uses an Atmel ATmega16. For the 
ATmega 16 version it is the idea to allow the extra I/O-ports to be 
used for scanning two C64 joysticks connected to these, and reporting 
these as joystick events. Unfortunately I have not yet been able to 
get the composite USB device working correctly, so currently the code
only supports the keyboard. There is therefore no functional difference
between the ATmega8 and ATmega16 versions. The code in m16keyjoy is
experimental, and does not currently work.

Both configurations use a 12MHz crystal, and USB interfacing 
implemented with very simple hardware. Apart from  that, the
microcontroller handles decoding and debouncing of the 66 keys on 
the C64 keyboard, and using Objective Development's firmware-only 
USB driver sends decoded scan-codes to the PC over low-speed USB.
For the ATmega16 version a composite device is created, allowing 
the unit to function as both a keyboard and a joystick/gamepad.

Objective Development's USB driver is a firmware-only implementation 
of the low speed USB standard (USB 1.1) which can be used on cheap
AVR microcontrollers from Atmel.  

A detailed description of the features and limitations of OBDEV's 
AVR-USB can be found in "usbdrv/usbdrv.h". More information is 
available on Objective Development's homepage: 
http://www.obdev.at/products/avrusb/


Building the hardware
---------------------

The hardware for the c64key is quite simple. Circuit diagrams are
included in the directory "circuit" in PDF and PNG formats. The USB
interface hardware is based on Mindaugas Milasauskas' MJoy application
(http://www.mindaugas.com/projects/MJoy/). Apart from that, the only
interesting things are the 12MHz crystal and the connector interfacing
to the C64 keyboard matrix.

No circuit board layout is published, mainly because the application
is so simple that it is easier to realize on a perf-board (strip-board),
but the diagram should provide all needed information to create your own
implementation, whether on a perf-board or with printed circuit boards.

The connector CON1 is used for programming the microcontroller. The
connections used are the same as the 6-pin ISP connector used by Atmel,
and available on their programmers (such as the STK500 kit).

Instead of the old behavior where PD1 could be used for UART debug, it
is now used for the caps lock LED (if you want this). Of course this 
can also be disabled if the UART debugging is needed.

Building the firmware
---------------------

The c64key firmware requires avr-gcc and avr-libc (see 
http://www.nongnu.org/avr-libc/ for more details).

Before attempting to compile, edit the Makefile to select the
desired keyboard mapping and programmer options.

For the easiest approach, you can use WinAVR under Windows, which is
a bundle of the required libraries and tool-chain packaged for
Windows. It even comes with a good editor (Programmers Notepad),
which was used for the development throughout this project.
WinAVR is available from http://winavr.sourceforge.net/.

After installing WinAVR you should be able to run the command
"make" from the directory where the source code is located, which
will build the firmware. The "Makefile" included with the project is
based on an example from the WinAVR distribution, and has support
for burning the firmware to an AVR device through a range of different
programmers supported by the AVRdude programming utility. With a few
modifications to the Makefile it should be possible to accomplish this
programming by issuing the command "make program". Of course these
commands can also be executed from within Programmers Notepad.

Of course you will need some programming hardware to connect between
the circuit and the PC. Several possibilities exist (and are supported
by AVRdude), including some very simple programmers that can be built
on a tight budget. During the experimentation and development of this
project, I have used an STK500 development board from Atmel. If you 
have the possibility of getting one of these, I can highly recommend it,
not only as a programmer, but for a wealth of experiments that can be
done on the board, before any hardware needs to be built.

It is important that the fuses of the microcontroller are set correctly
before the application will work. The most important thing is to configure
the device to use an external crystal. The AVR device is configured to
use the internal RC oscillator from the factory, but this is not
sufficiently accurate to handle the delicate timing required for USB
communication. Each of the Makefiles have a target called fuses, which
uses avrdude to set the fuses for the device (provided the avrdude 
settings in the Makefile have been configured in an appropriate way).

Be aware that the firmware provided here does not use OBDEV's USB 
vendor ID, since this may not be used for HID devices. Instead John 
Hyde's VID is used (same as OBDEV's HIDkeys example). John has aggreed
that his VID can be used for educational purposes, so please MAKE SURE
THIS FIRMWARE DOES NOT ESCAPE YOUR LAB (or get a different VID).


Files
-----
readme.txt              This file.
changes.txt             What has been changed in the different versions.
c64key.ppg              Project Group file for Programmers Notepad.
doc.txt                 Further documentation.
license.txt             The license this project is released under.
circuit/                Directory with circuit diagrams in PDF and PNG formats.
usbdrv/                 Directory with the AVR-USB driver and docs.
m8key/                  The firmware for the ATmega8 version.
m8key/m8key.pnproj      Programmers Notepad project file for ATmega8 version.
m8key/Makefile          Build-file for the GNU Make utility.
m8key/main.hex          Binary version of the ATmega8 firmware.
m8key/main.c            The main firmware functions (ATmega8 version).
m8key/usbconfig.h       Configuration file for the AVR-USB driver (ATmega8).
m16key/                 The firmware for the ATmega16 version.
m16key/m16key.pnproj    Programmers Notepad project file for ATmega16 version.
m16key/Makefile         Build-file for the GNU Make utility.
m16key/main.hex         Binary version of the ATmega16 firmware.
m16key/main.c           The main firmware functions (ATmega16 version).
m16key/usbconfig.h      Configuration file for the AVR-USB driver (ATmega16).
m16keyjoy/              Unfinished version with joystick support.
keymaps/                Keyboard maps for both versions.
                        


Documentation
-------------

The file "doc.txt" contains details about the function of the c64key
application, both in terms of keyboard mappings/bindings, as well as
information that proved useful during the development of the project
(such as C64 keyboard mappings etc.) These may come in handy if you want
a broader understanding of how the application works.


License information
-------------------

The c64key project is based on Objective Development's HIDkeys demo 
application, and although the restriction is mainly that any derivative
works should be published under an Open Source license, the additional
requirements by OBDEV are very resonable, and the entire project has
been released under this license. In other words you should feel free
to use the firmware here for any purpose, but any enhancements should
be given back to the community, in the form of a contribution to OBDEV's
project pages, with full documentation.

See the file license.txt for details.


Copyright
---------

The c64key project and documentation is Copyright (C) 2007 by
Mikkel Holm Olsen (AKA Spaceman Spiff). More information about the
project can be found on the homepage http://symlink.dk/projects/c64key/.

$Id: $