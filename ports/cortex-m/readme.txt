pico]OS port form Arm Cortex-M CPUs
-----------------------------------

This is a port of pico]OS to arm cortex-m3 and cortex-m0 chips.
It has been tested with GCC 4.8 as compiler on following boards:

- Olimex STM32-H103 header board
- Olimex STM32-P152 prototype board
- Olimex LPC-P1343 prototype board
- NXP LPC1114 on breadboard (DIP28 package!)

It has also been tested with Keil simulator with STM32F103 (cortex-m3) and
NXP LPC1114 (cortex-m0) cpus.

Note that port works only with CPUs that have CMSIS library available.
In your project's Makefile, set CMSIS_INCLUDES to point directory where 
CMSIS headers can be found. If you need any defines for CMSIS, those can
be put into CMSIS_DEFINES variable.

Your configuration directory (the one having poscfg.h header) must
have a header called pos_cmsis.h which should include main
CMSIS header file for your CPU (for example for stm32f10x it
will be #include "stm32f10x.h")

Cortex-M0 implementation is not highly elegant as SVC instruction
always traps through hard fault due to way interrupts are blocked.
