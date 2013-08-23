pico]OS port form Arm cpus
--------------------------

This is a port of pico]OS to different arm chips.
It has been tested with GCC 4.1.0 as compiler on following boards:

- Embedded Artists Quickstart ethernet board (see www.embeddedartists.com)
- Olimex LPC-E2129 development board (see www.olimex.com)

Both these boards use arm cpu from Philips LPC2000 family.

This port runs tasks in arm system mode (maybe user mode also someday?).
Interrupts have their own stacks for each mode. The context switching
is performed by handly stmfd/ldmfd instructions provided by arm CPU,
and ideas for this are taken from previous pico]OS arm port (form samsung
arm cpu), FreeBSD arm port and ethernut arm port. For details, see
comments on assembly source files.

Generic arm code is in arch_?.{c|s} files. Cpu-specific things 
are in cpu_?{c|s} files under lpc2000/samsung/arm7pid directories.
I took this separation approach to make it easier to add more
cpu variants, if someone is interested (I currently have only philips CPUs).

The files in samsung directory are converted from older arm port
made for pico]OS. Also, the files in arm7pid directory are converted
from port submitted as patch to sf.net. These ports compile, but are
currently untested, since I don't have the necessary hardware to test them.

Compiling for your board
------------------------
Directory "test" contains a small "blink leds" example,
which can be compiled for boards mentioned above (just
define LD_SCRIPT and BOARD depending on which board
you have).

Port includes gnu ld linker scripts for some cpus.
If your CPU has different flash / ram sizes, you can
still compile for it by making a copy of existing linker
script (these live in ports/arm/boot directory)
and editing memory sizes at beginning of
it to match yours. Remember to adjust the makefile too to
use new script. You can place the modified linker script into
your project directory.

Sizes for various arm stacks are controlled by 
linker script also, so if you need to adjust interrupt
stack size (or other arm stack size other than task stack)
it is necessary to edit the script.

If you want to compile C code in THUMB mode (giving
smaller code size) add THUMB=yes to your Makefile.

Adding new arm CPU family
---------------------
If you have a different arm cpu than already included
in this port, you can add support for it:

- copy files from lpc2000 (or samsung, arm7pid) directory
- modify p_pos_initArch in cpu_c.c (ie. initialize board & timer)
- modify irq handler in cpu_a.s
- if necessary, implement armSetDefaultIrqHandler in cpu_c.c
- if there is a need to something in very early in boot,
  look at boot/crt0.s (shoudn't normally be necessary)

List of files
-------------

arch_a.s		General arm assembly for context swicthing etc.
arch_a_macros.h		Context switching macros
arch_c.c		General arm support
boot/*			General arm startup and linker support for gnu-ld
lpc2000/*		Support for Philips LPC2000 arm chips
samsung/*		Support for SAMSUNG S3C2510A CPU(ARM940T core)
arm7pid/*		Support for arm7 pid cpu
test/*			Example code for Embedded Artists Quickstart
			ethernet board and Olimex LPC-E2129 board
