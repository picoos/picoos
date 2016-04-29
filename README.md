pico]OS
=======

This is the latest version of pico]OS realtime operating system.

It was originally created by Dennis Kuschel and Swen Moczarski,
see [original site at sourceforge][1] for details. The have 
given up on development of it. But as pico]OS is _great_ software,
I decided to pick up the maintenance and developent of it.

Compared to latest sf.net release (1.0.4) it has following updates:

- Power management API to provide framework for MCU power saving / sleeping features
- Suppression of timer interrupts during idle mode (tickless idle). Implementations are available for cortex-m/stm32 and MSP430 ports.
- stdarg support for nano layer printf-functions
- Makefile system uses GNU make pattern rules for source directory handling (otherwise projects that had many directories run into troubles)
- Pico & nano layers are augmented by [micro-layer][4], which contains support for filesystems (and some other things)

I'm also actively maintaining hardware support for some environments:

- Arm cortex-m0/m3/m4
- Texas Instruments MSP430
- Microchip PIC32
- NPX LPC2xxx (Arm7tdmi)
- Generic Unix (using ucontext(3))

Updated doxygen manual is available [here][3].

There is some additional information in [my blogs][2]. Additional libraries for
networking, 1-wire access etc. can be found in [my github account][5].

[1]: http://picoos.sf.net
[2]: http://stonepile.fi/tags/picoos
[3]: http://arizuu.github.io/picoos
[4]: http://github.com/AriZuu/picoos-micro
[5]: http://github.com/AriZuu

