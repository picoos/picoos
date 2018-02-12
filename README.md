pico]OS
=======

This is the latest version of pico]OS, a highly configurable and very
fast real time operating system. It targets a wide range of
architectures, from the  small 8 bit processors with very low memory
till huge architectures like 32 bit processors with lots of memory.
Please see the [documentation][3] for further information.

pico]OS was originally created by Dennis Kuschel and Swen Moczarski,
see [original site at sourceforge][1] for details. They have
given up on development of it. But as pico]OS is _great_ software,
I decided to pick up the maintenance and developent of it.

Compared to latest sf.net release (1.0.4) it has following updates:

- Power management API to provide framework for 
  MCU power saving / sleeping features
- Suppression of timer interrupts during idle mode (tickless idle). 
  Implementations are available for cortex-m/stm32 and MSP430 ports.
- stdarg support for nano layer printf-functions
- Makefile system uses GNU make pattern rules for source directory handling 
  (otherwise projects that had many directories run into troubles)
- Pico & nano layers are augmented by [micro-layer][4], which contains 
  support for filesystems (and some other things)

I'm also actively maintaining hardware support for some environments:

- Arm cortex-m0/m3/m4
- Texas Instruments MSP430
- Microchip PIC32
- NPX LPC2xxx (Arm7tdmi)
- Generic Unix (using ucontext(3))

There is some additional information in [my blogs][2]. Additional libraries for
networking, 1-wire access etc. can be found in [my github account][5].

Getting started
---------------

There are two methods to work with pico]OS. The first is to build the pico]OS
library and link it to your project that you may develop with some kind of
integrated compiler/debugger IDE. The second method is to use the Makefile
system that is provided with pico]OS to build all you need - the pico]OS RTOS
library, application libararies and your main program.

pico]OS makefiles need working GNU make for your host machine. For unix
systems it is usually easily installable from source code or prebuilt package.
For MS Windows, use of [MSYS2 package][6] is recommended.

Building example programs
-------------------------

When you have a working GNU make installed, you are now ready to build the
example programs that are shipped with pico]OS. Assuming your host is a
MS Windows machine and you have  MS Visual Studio or the MinGW GCC package
installed, you can compile the RTOS for MS Windows. Simply change into
the examples/app directory and enter "make PORT=x86w32". If you have trouble
building the examples, please read the readme file in the appropriated
port directory (eg. ports/x86w32/readme.txt). You will find the  generated
executables in the directory bin/x86w32-deb/out.

Building pico]OS as library
---------------------------

Library version of pico]OS might be useful when using
an IDE for development (other alternative is to include pico]OS
source files in IDE project in it's native way). To build the library,
execute make at pico]OS root directory.

The makefile takes two parameters:
 - PORT  = name of the port to build (= the subdirectory name)
 - BUILD = version to build, possible values are DEBUG and RELEASE

Example: make PORT=avr BUILD=DEBUG

Builds the Atmel AVR port and includes debug informations.
The generated library can be found in the directory lib/avr/deb/

The makefile knows the targets 'all', 'clean' and 'docu', and
'all' is the default.  'all' compiles the operating system.
'clean' removes all generated binaries. 'docu' generates the
html help with use of the doxygen tool.

Makefile searches for the configuration file 'config.mak' in
the pico]OS root directory. You can put in there your build parameters.

    #--- start of file ---
     PORT  = avr
     BUILD = DEBUG
    #---- end of file ----

Contributing to pico]OS development
-----------------------------------

Development takes place at [github][7].
To submit code, please follow these guidelines:

- fork the project before making changes
- create a git branch in forked project
- code must be under same [license][8] as rest of pico]OS (modified BSD license)
- format code in pico]OS style
  - indent with 2 spaces, no tabs
  - try to follow style of existing source files
- before submitting pull request consider squashing of your commits
- submit pull request

[1]: http://picoos.sf.net
[2]: http://stonepile.fi/tags/picoos
[3]: http://arizuu.github.io/picoos
[4]: http://github.com/AriZuu/picoos-micro
[5]: http://github.com/AriZuu
[6]: https://sourceforge.net/projects/msys2/
[7]: http://github.com/AriZuu/picoos
[8]: http://github.com/AriZuu/LICENSE

