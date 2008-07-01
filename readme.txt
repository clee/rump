rump (Real USB Model-M PCB) is Free Software; you can redistribute
and/or modify it under the terms of the OBDEV lice,nse, as found  
in the license.txt file.                                          
                                                                  
rump is distributed in the hope that it will be useful, but       
WITHOUT ANY WARRANTY; without even the implied warranty of        
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     
OBDEV license for further details.                                
                                                              

This is the README file for Real USB Model-M PCB (which might be a little
bit of a misnomer since, technically, devices running this code won't ever
pass the real USB certification test suite).

This document originally described Spaceman Spiff's Commodore 64 Keyboard;
it has been updated to (hopefully) more accurately reflect the changes made
to work with the IBM Model M instead.

Real USB Model-M PCB (or rump for short) is an implementation of a low-speed
USB HID device, specifically a keyboard. The physical appearance of the device
is that of a normal IBM Model M keyboard, but the device works as a USB
keyboard that can be used with any PC that does not have PS2 ports.

Objective Development's USB driver is a firmware-only implementation 
of the low speed USB standard (USB 1.1) which can be used on cheap
AVR microcontrollers from Atmel.  

A detailed description of the features and limitations of OBDEV's 
AVR-USB can be found in "usbdrv/usbdrv.h". More information is 
available on Objective Development's homepage: 
http://www.obdev.at/products/avrusb/


Building the firmware
---------------------

The rump firmware requires avr-gcc and avr-libc (see 
http://www.nongnu.org/avr-libc/ for more details).

Before attempting to compile, edit the Makefile to select the
desired programmer options.

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
project, I have used a USBtiny development board from Lady Ada 
<http://www.ladyada.net/make/usbtinyisp/>. 

If you have the possibility of getting one of these, I definitely
recommend it as a programmer; it doesn't have any fancy simulators
or debugging features (as far as I know) but it does provide bus power
to the target board being programmed, which made my life much easier.

It is important that the fuses of the microcontroller are set correctly
before the application will work. The most important thing is to configure
the device to use an external crystal. The AVR device is configured to
use the internal RC oscillator from the factory, but this is not
sufficiently accurate to handle the delicate timing required for USB
communication. Each of the Makefiles have a target called fuses, which
uses avrdude to set the fuses for the device (provided the avrdude 
settings in the Makefile have been configured in an appropriate way).

Be aware that the firmware provided here does not use OBDEV's USB 
vendor ID, since this may not be used for HID devices. Instead,
Spiff instructed me to use the VID/PID pair he uses for his SpiffChorder
keyboard project. At some point I'm hoping to get my own VID/PID pair,
but I'm not sure it makes sense to spend too much money on it.


Documentation
-------------

The file "doc.txt" contains details about the function of the rump 
application, both in terms of keyboard mappings/bindings, as well as
information that proved useful during the development of the project
(such as Model M keyboard mappings, etc.) These may come in handy if
you want a broader understanding of how the application works.


License information
-------------------

The rump project is based on c64key which is based on Objective 
Development's HIDkeys demo application, and although the restriction
is mainly that any derivative works should be published under an Open
Source license, the additional requirements by OBDEV are very resonable,
and the entire project has been released under this license. In other
words you should feel free to use the firmware here for any purpose,
but any enhancements should be given back to the community, in the form
of a contribution to OBDEV's project pages, with full documentation.

See the file license.txt for details.


Copyright
---------

The rump project and documentation is Copyright (C) 2008 by
Chris Lee <clee@mg8.org>. More information about the
project can be found on the homepage http://mg8.org/rump/.

All of this work was built on top of the great work that Spaceman Spiff
did for his c64key project, and none of this would have been possible
without AVR-USB from Objective Development.

c64key <http://symlink.dk/projects/c64key/>
AVR-USB <http://obdev.at/products/avrusb/index.html>
