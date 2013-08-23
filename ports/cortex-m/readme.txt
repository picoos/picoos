pico]OS port form Arm Cortex-M CPUs
-----------------------------------

This is a port of pico]OS to arm cortex-m3 and cortex-m0 chips.
It has been tested with Yagarto GCC 4.6 as compiler on following boards:

- Olimex STM32-H103 header board

It has also been tested with Keil simulator with STM32F103 (cortex-m3) and
NXP LPC1114 (cortex-m0) cpus.

Note that port works only with CPUs that have CMSIS library available.

Cortex-M0 implementation is not highly elegant as SVC instruction
always traps through hard fault due to way interrupts are blocked.
